#include "includes.h"

auto main() -> int {
    // instanciate driver handle 
    std::wstring TargetApplication = encrypt(L"FortniteClient-Win64-Shipping.exe");
    std::shared_ptr<driver::c_Memory> Memory = std::make_shared<driver::c_Memory>(TargetApplication);
    std::shared_ptr<ui::c_render> Rendering = std::make_shared<ui::c_render>(); // for drawing

    // grab offsets online
    if (!Offsets::GrabOffsets())
    {
        std::cout << "Couldnt get offsets" << std::endl;
        return false;
    }

    std::shared_ptr<features::c_Features> Features = std::make_unique<features::c_Features>(Rendering, Memory);
    std::unique_ptr<ui::c_UI> Menu = std::make_unique<ui::c_UI>(Rendering, Features);

    // reading takes time so offload it to a seperate thread
    std::thread cache([&Memory]()
    {
        sdk::RefreshCache(Memory);
    });
    
    while (true)
    {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                break;
        }
        
        if (!Menu->isMenuInitialized || !PtrCache::Gworld)
        {
            std::string msg = (!PtrCache::Gworld) ? encrypt("Cannot get uworld") : encrypt("Cannot init menu");
            std::cout << msg << "\n";
            Sleep(500);
            continue;
        }

        Features->onUpdate(); // get data to render and cache it
        Menu->onRender(); // calls Features->onRender() after dx9 init and before endframe

        Sleep(5);
    }

    cache.detach();

    std::cout << encrypt("Press enter to exit...") << std::endl;
    std::cin.get();

    return 0;
}