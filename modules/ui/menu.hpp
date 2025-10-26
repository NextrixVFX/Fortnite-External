#pragma once
#include <windowsx.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static bool bMenuOpen = true;

namespace ui
{
    static UINT g_ResizeWidth = 0, g_ResizeHeight = 0;
    static HWND hwnd = nullptr;
    static WNDCLASSEXW wc = WNDCLASSEXW{};
    static IDirect3D9Ex* pD3D = nullptr;
    static IDirect3DDevice9Ex* pDevice = nullptr;
    static D3DPRESENT_PARAMETERS d3dParams = {};

    LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
            return true;

        // Get ImGui IO once at the top
        ImGuiIO& io = ImGui::GetIO();

        switch (msg)
        {
        case WM_SIZE:
            if (wParam == SIZE_MINIMIZED)
                return 0;
            g_ResizeWidth = (UINT)LOWORD(lParam);
            g_ResizeHeight = (UINT)HIWORD(lParam);
            return 0;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU)
                return 0;
            break;
        case WM_MOUSEMOVE:
            io.MousePos = ImVec2((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam));
            break;
        case WM_NCHITTEST:
        {
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            ScreenToClient(hWnd, &pt);
            io.MousePos = ImVec2((float)pt.x, (float)pt.y);

            if (io.WantCaptureMouse) {
                return HTCLIENT;
            }
            return HTTRANSPARENT;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }
        return DefWindowProcW(hWnd, msg, wParam, lParam);
    }
    
    class c_UI
    {
    private:
    public:
        std::shared_ptr<ui::c_render> Rendering = nullptr;
        std::unique_ptr<features::c_ESP> Esp = nullptr;
        std::unique_ptr<features::c_Softaim> Softaim = nullptr;
        bool isMenuInitialized = false;
        
        c_UI(std::shared_ptr<driver::c_Memory> Memory_ptr)
        {
            // Create overlay window
            wc = { 
                sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), 
                nullptr, nullptr, nullptr, nullptr, encrypt(L"SuperScaryOverlay"), nullptr 
            }; 
            
            RegisterClassExW(&wc); 

            hwnd = CreateWindowExW(
                WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_NOACTIVATE,
                wc.lpszClassName,
                encrypt(L"SuperScaryOverlay"),
                WS_POPUP,
                0, 0,
                GetSystemMetrics(SM_CXSCREEN),
                GetSystemMetrics(SM_CYSCREEN),
                nullptr, nullptr, wc.hInstance, nullptr
            );

            SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
            
            ShowWindow(hwnd, SW_SHOWDEFAULT);
            UpdateWindow(hwnd);

            if (!InitDirectX())
            {
                Cleanup();
                return;
            }

            Rendering = std::make_shared<ui::c_render>();
            Esp = std::make_unique<features::c_ESP>();
            Softaim = std::make_unique<features::c_Softaim>();

            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            io.IniFilename = nullptr;
            io.LogFilename = nullptr;
            io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

            Rendering->Initialize();
            Esp->Initialize(Rendering, Memory_ptr);
            Softaim->Initialize(Rendering, Memory_ptr);

            ImGui::StyleColorsDark();
            ImGui_ImplWin32_Init(hwnd);
            ImGui_ImplDX9_Init(pDevice);

            isMenuInitialized = true;
        }

        ~c_UI()
        {
            Cleanup();
        }

    private:
        bool InitDirectX()
        {
            if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &pD3D)))
                return false;

            ZeroMemory(&d3dParams, sizeof(d3dParams));
            d3dParams.Windowed = TRUE;
            d3dParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
            d3dParams.BackBufferFormat = D3DFMT_A8R8G8B8;
            d3dParams.BackBufferWidth = GetSystemMetrics(SM_CXSCREEN);
            d3dParams.BackBufferHeight = GetSystemMetrics(SM_CYSCREEN);
            d3dParams.EnableAutoDepthStencil = TRUE;
            d3dParams.AutoDepthStencilFormat = D3DFMT_D16;
            d3dParams.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
            d3dParams.hDeviceWindow = hwnd;

            if (FAILED(pD3D->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
                D3DCREATE_HARDWARE_VERTEXPROCESSING,
                &d3dParams, 0, &pDevice)))
                return false;

            return true;
        }

        void Cleanup()
        {
            ImGui_ImplDX9_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();

            if (pDevice) { pDevice->Release(); pDevice = nullptr; }
            if (pD3D) { pD3D->Release(); pD3D = nullptr; }
            if (hwnd) { DestroyWindow(hwnd); hwnd = nullptr; }
            UnregisterClassA(encrypt("SuperScaryOverlay"), wc.hInstance);
        }

    public:
        void RenderLoop()
        {
            ImGui_ImplDX9_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            Rendering->GetDrawList() = ImGui::GetBackgroundDrawList();

            // Render menu
            RenderMenu();

            // Render ESP
            if (Esp && Esp->isInitialized)
                Esp->onRender();

            // Render Softaim
            if (Softaim && Softaim->isInitialized && useSoftaim)
                Softaim->onRender();

            //ImGui::EndFrame();
            pDevice->SetRenderState(D3DRS_ZENABLE, false);
            pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
            pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
            pDevice->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

            if (pDevice->BeginScene() >= 0)
            {
                ImGui::Render();
                ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
                pDevice->EndScene();
            }

            HRESULT result = pDevice->Present(nullptr, nullptr, nullptr, nullptr);
            if (result == D3DERR_DEVICELOST && pDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            {
                ImGui_ImplDX9_InvalidateDeviceObjects();
                pDevice->Reset(&d3dParams);
                ImGui_ImplDX9_CreateDeviceObjects();
            }
        }

        void RenderMenu()
        {
            static int MenuTab = 0;
            if (GetAsyncKeyState(VK_INSERT) & 1) bMenuOpen = !bMenuOpen;

            if (!bMenuOpen) return;

            ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Once);
            ImGui::Begin(encrypt("Menu"), &bMenuOpen);

            if (ImGui::Button(encrypt("Softaim"), ImVec2(80, 0))) MenuTab = 0;
            ImGui::SameLine();
            if (ImGui::Button(encrypt("Visuals"), ImVec2(80, 0))) MenuTab = 1;
            ImGui::SameLine();
            if (ImGui::Button(encrypt("Colors"), ImVec2(80, 0))) MenuTab = 2;
            
            ImGui::Separator();

            if (MenuTab == 0)
            {
                ImGui::Checkbox(encrypt("Enable Softaim"), &useSoftaim);
                ImGui::SliderFloat(encrypt("Smoothness"), &gotAimSmoothness, 0, 10);
                ImGui::SliderInt(encrypt("FOV"), &gotAimFOV, 100, 400);
            }
            else if (MenuTab == 1)
            {
                ImGui::Checkbox(encrypt("Enable ESP"), &useESP);
                ImGui::Checkbox(encrypt("Use Boxes"), &useBoxes);
                ImGui::SameLine();
                ImGui::Checkbox(encrypt("Use Lines"), &useLines);
                ImGui::SliderFloat(encrypt("Line Height"), &lineHeight, 0.0f, 1.0f);
                ImGui::SliderInt(encrypt("Target Bone"), &selectedBone, 0, 240);
                ImGui::SliderInt(encrypt("FOV"), &gotFOV, 70, 105);
            }
            else if (MenuTab == 2)
            {
                ImGui::ColorPicker4(encrypt("FOV Color"), fovColor);
                ImGui::ColorPicker4(encrypt("Box Color"), boxColor);
                ImGui::ColorPicker4(encrypt("Line Color"), lineColor);
            }

            ImGui::End();
        }
    };
}
