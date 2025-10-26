#include "includes.h"

static std::shared_ptr<driver::c_Memory> Memory = nullptr;
static std::unique_ptr<ui::c_UI> Menu = nullptr;
static std::unique_ptr < features::c_Features> Features = nullptr;



auto main() -> int {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

    // instanciate driver handle 
    Memory = std::make_shared<driver::c_Memory>(encrypt(L"FortniteClient-Win64-Shipping.exe"));

    // grab offsets online
    if (!Offsets::GrabOffsets())
    {
        std::cout << "Couldnt get offsets" << std::endl;
        return false;
    }

    // read all pointer offsets ina seperate thread for optimization
    std::thread cache(sdk::RefreshCache, Memory);

    Menu = std::make_unique<ui::c_UI>(Memory);
    Features = std::make_unique<features::c_Features>();
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

        if (!PtrCache::Gworld)
        {
            Sleep(500);
            continue;
        }

        if (Menu->isMenuInitialized)
            Menu->RenderLoop();

        Sleep(5);
    }

    cache.join();

    std::cout << encrypt("Press enter to exit...") << std::endl;
    std::cin.get();

    return 0;
}