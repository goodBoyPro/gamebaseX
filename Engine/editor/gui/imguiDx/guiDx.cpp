
#include "guiDx.h"
#include"filesystem"
// 显式声明ImGui Win32处理函数
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam,
                                                             LPARAM lParam);

// 全局D3D资源
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGIFactory* g_pFactory = nullptr;

// 全局字体列表
static std::vector<FontInfo> g_fonts = {
    {"res/font/heiti.ttf", 16.0f, "黑体", nullptr, {}}           // 系统黑体
    // {"res/font/heiti.ttf", 16.0f, "宋体", nullptr, {}},    // 系统宋体
    // {"res/font/heiti.ttf", 16.0f, "微软雅黑", nullptr, {}},  // 系统微软雅黑
    // {"res/font/heiti.ttf", 16.0f, "Arial", nullptr, {}}     // 系统Arial
};
static ImFontAtlas* g_globalFontAtlas = nullptr;
bool PreloadGlobalFonts() {
    // 1. 创建全局字体Atlas（所有窗口共享）
    g_globalFontAtlas = IM_NEW(ImFontAtlas);
    if (!g_globalFontAtlas) {
        printf("错误：创建全局字体Atlas失败\n");
        return false;
    }

    // 2. 加载所有字体文件到全局Atlas
    for (auto& font : g_fonts) {
        // 检查文件是否存在
        if (!std::filesystem::exists(font.filePath)) {
            printf("错误：字体文件不存在 %s\n", font.filePath);
            IM_DELETE(g_globalFontAtlas);
            continue;;
        }

        ImFontConfig config;
        config.MergeMode = false;
        config.PixelSnapH = true;
        config.FontDataOwnedByAtlas = true;

        // 中文字体范围
        const ImWchar ranges[] = {0x0020, 0x00FF,  // 基本ASCII
                                  0x4E00, 0x9FA5,  // 常用汉字
                                  0};

        // 从文件加载字体到全局Atlas
        font.font =
            g_globalFontAtlas->AddFontFromFileTTF(font.filePath, font.size, &config, ranges);

        if (!font.font) {
            printf("警告：无法创建字体 %s\n", font.name);
            IM_DELETE(g_globalFontAtlas);
            g_globalFontAtlas = nullptr;
            continue;
        }
    }

    // 3. 构建字体纹理（仅1次）
    return g_globalFontAtlas->Build();
}
void CleanupGlobalFonts() {
    if (g_globalFontAtlas) {
        IM_DELETE(g_globalFontAtlas);
        g_globalFontAtlas = nullptr;
    }

    // 清空字体数据缓存
    for (auto& font : g_fonts) {
        font.fontData.clear();
        font.font = nullptr;
    }
}
// 函数声明
bool CreateDeviceD3D();
bool CreateSwapChainForWindow(HWND hWnd, IDXGISwapChain** ppSwapChain);
void CleanupDeviceD3D();
LRESULT WINAPI SharedWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool RegisterSharedWindowClass(const WCHAR* className);
bool LoadCustomFonts(ImGuiIO& io);
HWND CreateWindowWithPrivateData(const WCHAR* className, const WCHAR* title, int x, int y,
                                 int width, int height, ImGuiContext** outImguiCtx,
                                 WindowPrivateData** outWndData);
void CleanupWindowResources(HWND hWnd, ImGuiContext* ctx, WindowPrivateData* data);

// D3D设备创建
bool CreateDeviceD3D() {
    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = {D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0};

    HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags,
                                   featureLevelArray, 2, D3D11_SDK_VERSION, &g_pd3dDevice,
                                   &featureLevel, &g_pd3dDeviceContext);
    if (hr != S_OK) return false;

    // 获取DXGI工厂
    IDXGIDevice* dxgiDevice = nullptr;
    if (g_pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice) != S_OK) {
        return false;
    }

    IDXGIAdapter* dxgiAdapter = nullptr;
    if (dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter) != S_OK) {
        dxgiDevice->Release();
        return false;
    }

    if (dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&g_pFactory) != S_OK) {
        dxgiAdapter->Release();
        dxgiDevice->Release();
        return false;
    }

    dxgiAdapter->Release();
    dxgiDevice->Release();
    return true;
}

// 创建窗口交换链
bool CreateSwapChainForWindow(HWND hWnd, IDXGISwapChain** ppSwapChain) {
    if (!g_pd3dDevice || !g_pFactory) return false;

    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    HRESULT hr = g_pFactory->CreateSwapChain(g_pd3dDevice, &sd, ppSwapChain);
    return hr == S_OK;
}

// 清理D3D设备
void CleanupDeviceD3D() {
    if (g_pFactory) {
        g_pFactory->Release();
        g_pFactory = nullptr;
    }
    if (g_pd3dDeviceContext) {
        g_pd3dDeviceContext->Release();
        g_pd3dDeviceContext = nullptr;
    }
    if (g_pd3dDevice) {
        g_pd3dDevice->Release();
        g_pd3dDevice = nullptr;
    }
}

// 共享窗口回调函数
LRESULT WINAPI SharedWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    WindowPrivateData* pWndData =
        reinterpret_cast<WindowPrivateData*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    // 处理ImGui消息
    if (pWndData != nullptr && pWndData->imguiContext != nullptr) {
        ImGui::SetCurrentContext(pWndData->imguiContext);
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) {
            return true;
        }
    }

    // 处理窗口消息
    switch (msg) {
        case WM_SIZE:
            if (pWndData != nullptr && g_pd3dDevice != nullptr && wParam != SIZE_MINIMIZED) {
                if (pWndData->rtv) {
                    pWndData->rtv->Release();
                    pWndData->rtv = nullptr;
                }

                UINT newWidth = static_cast<UINT>(LOWORD(lParam));
                UINT newHeight = static_cast<UINT>(HIWORD(lParam));
                pWndData->swapChain->ResizeBuffers(0, newWidth, newHeight, DXGI_FORMAT_UNKNOWN, 0);

                ID3D11Texture2D* pBackBuffer = nullptr;
                if (pWndData->swapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer)) == S_OK) {
                    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pWndData->rtv);
                    pBackBuffer->Release();
                }
            }
            return 0;

        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) {
                return 0;
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

// 注册共享窗口类
bool RegisterSharedWindowClass(const WCHAR* className) {
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_CLASSDC;
    wc.lpfnWndProc = SharedWndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = className;

    if (!RegisterClassEx(&wc)) {
        if (GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
            return false;
        }
    }
    return true;
}
////////////////////////////////////////////////////////////////////////////////

// 加载自定义字体
bool LoadCustomFonts(ImGuiIO& io) {
    // 检查全局字体是否已预加载
    if (!g_globalFontAtlas) {
        printf("警告：全局字体未预加载，使用默认字体\n");
        io.Fonts->AddFontDefault();
        io.Fonts->Build();
        return false;
    }

    // 直接使用全局字体 atlas 替代当前上下文的 atlas
    io.Fonts = g_globalFontAtlas;

    return true;
}

// 创建带私有数据的窗口
HWND CreateWindowWithPrivateData(const WCHAR* className, const WCHAR* title, int x, int y,
                                 int width, int height, ImGuiContext** outImguiCtx,
                                 WindowPrivateData** outWndData) {
    HWND hWnd = CreateWindowExW(WS_EX_LAYERED, className, title, WS_POPUP, x, y, width,
                                height, nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
    if (hWnd == nullptr) return nullptr;
   
    *outWndData = new WindowPrivateData();
    (*outWndData)->className = className;
    (*outWndData)->imguiContext = nullptr;
    (*outWndData)->rtv = nullptr;
    (*outWndData)->swapChain = nullptr;
    (*outWndData)->fontIndex = 0;  // 默认使用第一个字体

    if (!CreateSwapChainForWindow(hWnd, &(*outWndData)->swapChain)) {
        delete *outWndData;
        DestroyWindow(hWnd);
        return nullptr;
    }

    ID3D11Texture2D* pBackBuffer = nullptr;
    if ((*outWndData)->swapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer)) == S_OK) {
        g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &(*outWndData)->rtv);
        pBackBuffer->Release();
    }

    *outImguiCtx = ImGui::CreateContext();
    (*outWndData)->imguiContext = *outImguiCtx;
    ImGui::SetCurrentContext(*outImguiCtx);

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    // 加载自定义字体
    if (!LoadCustomFonts(io)) {
        // 加载失败时使用默认字体
        io.Fonts->AddFontDefault();
        io.Fonts->Build();
    }

    // ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 4.0f;
    style.FrameRounding = 2.0f;
    style.Colors[ImGuiCol_Text] = ImVec4(0.1, 0.1, 0.1, 1);

    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(*outWndData));

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    return hWnd;
}

// 清理窗口资源
void CleanupWindowResources(HWND hWnd, ImGuiContext* ctx, WindowPrivateData* data) {
    if (ctx != nullptr) {
        ImGui::SetCurrentContext(ctx);
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext(ctx);
    }

    if (data != nullptr) {
        if (data->rtv) {
            data->rtv->Release();
        }
        if (data->swapChain) {
            data->swapChain->Release();
        }
        delete data;
    }

    if (hWnd != nullptr) {
        DestroyWindow(hWnd);
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void BigWindow::loop() {
    bool done = false;
    MSG msg = {};
    if (!done) {
        while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT) done = true;
        }
        if (done) return;

        // 渲染窗口1
        ImGui::SetCurrentContext(ctx);
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // // 设置当前字体
        // ImGui::PushFont(g_fonts[data->fontIndex].font);

        // ImGui::SetNextWindowPos(ImVec2(0, 0));
        // ImGui::SetNextWindowSize(ImVec2(800, 600));
        // ImGui::Begin(
        //     "窗口 1", nullptr,
        //     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

        // ImGui::Text("这是窗口1 - 自定义字体示例");
        // ImGui::Text("当前字体: %s", g_fonts[data->fontIndex].name);
        // ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

        // // 字体选择下拉框
        // if (ImGui::BeginCombo("选择字体", g_fonts[data->fontIndex].name)) {
        //     for (size_t i = 0; i < g_fonts.size(); i++) {
        //         bool isSelected = (data->fontIndex == i);
        //         if (ImGui::Selectable(g_fonts[i].name, isSelected)) {
        //             data->fontIndex = i;
        //         }
        //         if (isSelected) {
        //             ImGui::SetItemDefaultFocus();
        //         }
        //     }
        //     ImGui::EndCombo();
        // }

        // ImGui::Separator();
        // ImGui::Text("中文显示测试：你好，世界！");
        // ImGui::Text("English Test: Hello, World!");

        // ImGui::End();
        // ImGui::PopFont();  // 恢复默认字体
        /////////////////////////////////////////////////////////////
        for (auto& miniw : allWindows) {
            miniw.second->loop();
        }
        followParent();
        /////////////////////////////////////////////////////////////
        g_pd3dDeviceContext->OMSetRenderTargets(1, &data->rtv, nullptr);
        const float clearColor1[] = {0, 0, 0, 0};
        g_pd3dDeviceContext->ClearRenderTargetView(data->rtv, clearColor1);
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        data->swapChain->Present(1, 0);
    }
}
BigWindow::~BigWindow() {
    for (auto& miniw : allWindows) {
        delete miniw.second;
    }
    CleanupWindowResources(hWnd, ctx, data);
}
UIManager::UIManager() {
    if (!CreateDeviceD3D()) {
        MessageBox(nullptr, L"创建D3D设备失败！", L"错误", MB_ICONERROR);
        CleanupDeviceD3D();
        throw(std::runtime_error("CreateDeviceD3D failed\n"));
    }
    PreloadGlobalFonts();

    if (!RegisterSharedWindowClass(sharedClassName)) {
        MessageBox(nullptr, L"注册窗口类失败！", L"错误", MB_ICONERROR);
        CleanupDeviceD3D();
        throw(std::runtime_error("register windowclass failed\n"));
    }
}
BigWindow* UIManager::createBigWindow(const wchar_t*windowName,HWND parent_) {
    BigWindow* ret = new BigWindow;
    ret->hWnd = CreateWindowWithPrivateData(sharedClassName, windowName, 100, 100, 800, 600,
                                            &(ret->ctx), &(ret->data));
    if (ret->hWnd == nullptr) {
        throw(std::runtime_error("hwnd create failed\n"));
    }
    if(parent_){ret->setParentWindow(parent_);}
    allWindows.push_back(ret);

    return ret;
}
UIManager::~UIManager() {
    for (BigWindow* window : allWindows) {
        delete window;
    }
    CleanupGlobalFonts();
    UnregisterClass(sharedClassName, GetModuleHandle(nullptr));
    CleanupDeviceD3D();
}
void UIManager::MainLoop() {
    for (BigWindow* window : allWindows) {
        window->loop();
    }
}
//强制初始化d3d环境
struct UIManagerInitStruct{
  UIManagerInitStruct(){getUiManager();}
};
static UIManagerInitStruct UIManagerInitStructInst;