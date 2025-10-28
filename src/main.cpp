#include "includes.h"

static std::shared_ptr<driver::c_Memory> Memory = nullptr;
static std::shared_ptr<ui::c_render> Rendering = nullptr;

auto main() -> int {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

    // instanciate driver handle 
    Memory = std::make_shared<driver::c_Memory>(encrypt(L"FortniteClient-Win64-Shipping.exe"));
    Rendering = std::make_shared<ui::c_render>();

    // grab offsets online
    if (!Offsets::GrabOffsets())
    {
        std::cout << "Couldnt get offsets" << std::endl;
        return false;
    }

    std::shared_ptr<features::c_Features> Features = std::make_unique<features::c_Features>(Rendering, Memory);
    
    // reading takes time so offload it to a seperate thread
    std::thread cache(sdk::RefreshCache, Memory);
    
    // seperate rendering because its cleaner imo
    std::thread render([&]() {
        std::unique_ptr<ui::c_UI> Menu = std::make_unique<ui::c_UI>(Rendering, Features);
        Menu->RenderLoop();
    });

    while (true)
    {
        if (!PtrCache::Gworld)
        {
            Sleep(500);
            continue;
        }

        Features->onUpdate();

        Sleep(1);
    }

    cache.join();
    render.join();

    std::cout << encrypt("Press enter to exit...") << std::endl;
    std::cin.get();

    return 0;
}