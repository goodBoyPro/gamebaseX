// 封装imgui
/*
1.编译时需要链接  d3d11 dwmapi d3dcompiler
在imgui文件夹中有cpp文件，不要忘记编译
2.在目录中创建settings文件夹，存储ingui.ini配置
3.imgui的特殊语法"button##id",##后面的内容不显示，同时又能区分同名按钮
*/
#if !defined(GUIFROMIMGUI_H)
#define GUIFROMIMGUI_H
#include <d3d11.h>
#include <tchar.h>

#include <filesystem>
#include <functional>
#include <map>
#include <string>

#include "imguiLib/imgui.h"
#include "imguiLib/imgui_impl_dx11.h"
#include "imguiLib/imgui_impl_win32.h"
class guiFromImgui {
public:
  struct DoOnce;
  static guiFromImgui &getUi();
  void close() { bOpen = false; };
  bool isOpen() { return bOpen; }
  // 在应用的主循环中调用这个方法
  void mainLoop();
  //////小窗口///////////////////////////////////////////////////////////////////////////////////////////////////
  std::map<std::string, class UiWindow *> allWindows;
  template <class T>
  UiWindow &createWindow(const std::string &id, int state = 0);

  ~guiFromImgui();
  void setFollowOtherWindow() {
    SetLayeredWindowAttributes(hwnd,
                               ImColor(clear_color_with_alpha[0] * 255,
                                       clear_color_with_alpha[1] * 255,
                                       clear_color_with_alpha[2] * 255),
                               NULL, LWA_COLORKEY);
    
  }
  void followOtherWindow(HWND hwnd_) {
    RECT rect;
    GetClientRect(hwnd_, &rect);
    POINT point = {rect.left, rect.top};
    ClientToScreen(hwnd_, &point);
    MoveWindow(hwnd, point.x, point.y, rect.right, rect.bottom, true);
  }

private:
  float clear_color_with_alpha[4] = {0, 0, 0, 1};
  bool bOpen = true;
  inline static ID3D11Device *g_pd3dDevice = nullptr;
  inline static ID3D11DeviceContext *g_pd3dDeviceContext = nullptr;
  inline static IDXGISwapChain *g_pSwapChain = nullptr;
  inline static bool g_SwapChainOccluded = false;
  inline static UINT g_ResizeWidth = 0, g_ResizeHeight = 0;
  inline static ID3D11RenderTargetView *g_mainRenderTargetView = nullptr;
  HWND hwnd;
  WNDCLASSEXW wc = {sizeof(wc),
                    CS_CLASSDC,
                    WndProc,
                    0L,
                    0L,
                    GetModuleHandle(nullptr),
                    nullptr,
                    nullptr,
                    nullptr,
                    nullptr,
                    L"ImGui Example",
                    nullptr};

private:
  guiFromImgui(const wchar_t *windowName = L"window");
  bool CreateDeviceD3D(HWND hWnd);
  void CleanupDeviceD3D();
  void CreateRenderTarget();
  void CleanupRenderTarget();
  static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam,
                                LPARAM lParam);
  void createIfNoDir();
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////
class UiWindow {
public:
  UiWindow(const std::string &id_, int state_) {
    id = id_;
    state = state_;
    ImGuiStyle &style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Text]=ImVec4(0.1,0.1,0.1,1);
  };
  virtual ~UiWindow() { printf("uncons\n"); };
  std::function<void()> window = []() {};

public:
  std::string id;
  bool bOpen = true;

  int state = 0;
  // 这里分开写是因为需要在lamda表达式内部调用自身
  void setWindowUi(const std::function<void()> &callback) {
    window = callback;
  };
  virtual void loop() {
    if ((bOpen)) {
      ImGui::Begin(id.c_str(), &bOpen, state);
      window();
      ImGui::End();
    }
  }
};
///////////////////////////////////////////////////////////////////////////////////////////////////////
class Menus : public UiWindow {
public:
  Menus(const std::string &id_, int state_) : UiWindow(id_, state_) {}
  void loop() override {
    if ((bOpen)) {
      ImGui::Begin("windowXXX", &bOpen, ImGuiWindowFlags_MenuBar);
      ImGui::BeginChild("child", ImVec2(40, 40), true);
      ImGui::EndChild();

      if (ImGui::BeginMenuBar()) {
        window();
        ImGui::EndMenuBar();
      }
      ImGui::End();
    }
  }
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
inline guiFromImgui::guiFromImgui(const wchar_t *windowName) {
  // 建立配置文件所在文件夹
  createIfNoDir();
  // Create application window
  // ImGui_ImplWin32_EnableDpiAwareness();

  ::RegisterClassExW(&wc);
  // 主窗口是否透明
  bool isOpacity = true;
  if (!isOpacity) {
    hwnd = ::CreateWindowW(wc.lpszClassName, windowName, WS_OVERLAPPEDWINDOW,
                           100, 100, 1280, 800, nullptr, nullptr, wc.hInstance,
                           nullptr);
  } else {
    hwnd = ::CreateWindowExW(WS_EX_TOPMOST | WS_EX_LAYERED, wc.lpszClassName,
                             windowName, WS_POPUP, 100, 100, 1280, 800, nullptr,
                             nullptr, wc.hInstance, nullptr);
    setFollowOtherWindow();
  }

  // Initialize Direct3D
  if (!CreateDeviceD3D(hwnd)) {
    CleanupDeviceD3D();
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
    printf("error:window created failed");
  }

  // Show the window
  ::ShowWindow(hwnd, SW_SHOWDEFAULT);
  ::UpdateWindow(hwnd);

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

  // ImGui::StyleColorsDark();
  ImGui::StyleColorsLight();

  // Setup Platform/Renderer backends
  ImGui_ImplWin32_Init(hwnd);
  ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

  ImFont *font =
      io.Fonts->AddFontFromFileTTF("res/font/heiti.ttf", 20.0f, nullptr,
                                   io.Fonts->GetGlyphRangesChineseFull());
  IM_ASSERT(font);
}

inline guiFromImgui &guiFromImgui::getUi() {
  static guiFromImgui ui;
  return ui;
}

inline guiFromImgui::~guiFromImgui() {
  // Cleanup
  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();

  CleanupDeviceD3D();
  ::DestroyWindow(hwnd);
  ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
}

inline void guiFromImgui::mainLoop() {
  // Main loop
  if (bOpen) {
    // Poll and handle messages (inputs, window resize, etc.)
    // See the WndProc() function below for our to dispatch events to the
    // Win32 backend.
    MSG msg;
    while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
      ::TranslateMessage(&msg);
      ::DispatchMessage(&msg);
      if (msg.message == WM_QUIT)
        close();
    }

    // Handle window being minimized or screen locked
    if (g_SwapChainOccluded &&
        g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED) {
      ::Sleep(10);
      return;
    }
    g_SwapChainOccluded = false;

    // Handle window resize (we don't resize directly in the WM_SIZE
    // handler)
    if (g_ResizeWidth != 0 && g_ResizeHeight != 0) {
      CleanupRenderTarget();
      g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight,
                                  DXGI_FORMAT_UNKNOWN, 0);
      g_ResizeWidth = g_ResizeHeight = 0;
      CreateRenderTarget();
    }

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ///////////////////////////////////////////////////
    // windows();
    for (auto &window : allWindows) {
      window.second->loop();
    }
    ///////////////////////////////////////////////////
    // Rendering
    ImGui::Render();

    g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView,
                                            nullptr);
    g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView,
                                               clear_color_with_alpha);

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Present
    HRESULT hr = g_pSwapChain->Present(1, 0); // Present with vsync
    // HRESULT hr = g_pSwapChain->Present(0, 0); // Present without vsync
    g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
  }
}
template <class T>
inline UiWindow &guiFromImgui::createWindow(const std::string &id, int state) {
  if (allWindows.find(id) != allWindows.end()) {
    printf("error:window id redefined");
    assert(false);
  }
  UiWindow *window = new T(id, state);
  allWindows[id] = window;
  return *window;
}

inline bool guiFromImgui::CreateDeviceD3D(HWND hWnd) {
  // Setup swap chain
  DXGI_SWAP_CHAIN_DESC sd;
  ZeroMemory(&sd, sizeof(sd));
  sd.BufferCount = 2;
  sd.BufferDesc.Width = 0;
  sd.BufferDesc.Height = 0;
  sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  sd.BufferDesc.RefreshRate.Numerator = 60;
  sd.BufferDesc.RefreshRate.Denominator = 1;
  sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.OutputWindow = hWnd;
  sd.SampleDesc.Count = 1;
  sd.SampleDesc.Quality = 0;
  sd.Windowed = TRUE;
  sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

  UINT createDeviceFlags = 0;
  // createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
  D3D_FEATURE_LEVEL featureLevel;
  const D3D_FEATURE_LEVEL featureLevelArray[2] = {
      D3D_FEATURE_LEVEL_11_0,
      D3D_FEATURE_LEVEL_10_0,
  };
  HRESULT res = D3D11CreateDeviceAndSwapChain(
      nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags,
      featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain,
      &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
  if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software
                                     // driver if hardware is not available.
    res = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags,
        featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain,
        &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
  if (res != S_OK)
    return false;

  CreateRenderTarget();
  return true;
}

inline void guiFromImgui::CleanupDeviceD3D() {
  CleanupRenderTarget();
  if (g_pSwapChain) {
    g_pSwapChain->Release();
    g_pSwapChain = nullptr;
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

inline void guiFromImgui::CreateRenderTarget() {
  ID3D11Texture2D *pBackBuffer;
  g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
  g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr,
                                       &g_mainRenderTargetView);
  pBackBuffer->Release();
}

inline void guiFromImgui::CleanupRenderTarget() {
  if (g_mainRenderTargetView) {
    g_mainRenderTargetView->Release();
    g_mainRenderTargetView = nullptr;
  }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
                                                             UINT msg,
                                                             WPARAM wParam,
                                                             LPARAM lParam);
inline LRESULT WINAPI guiFromImgui::WndProc(HWND hWnd, UINT msg, WPARAM wParam,
                                            LPARAM lParam) {
  if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
    return true;

  switch (msg) {
  case WM_SIZE:
    if (wParam == SIZE_MINIMIZED)
      return 0;
    g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
    g_ResizeHeight = (UINT)HIWORD(lParam);
    return 0;
  case WM_SYSCOMMAND:
    if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
      return 0;
    break;
  case WM_DESTROY:
    ::PostQuitMessage(0);
    return 0;
  }
  return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

inline void guiFromImgui::createIfNoDir() {
  std::filesystem::path workingdir = std::filesystem::current_path();
  std::filesystem::path settingsDir = workingdir / "settings";
  if (std::filesystem::exists(settingsDir))
    return;

  std::filesystem::create_directory(settingsDir);
}
struct guiFromImgui::DoOnce {
  DoOnce(const std::function<void()> &cbk_) { cbk = cbk_; }

  void operator()() {
    if (isDone)
      return;
    cbk();
    isDone = true;
  }
  void reset() { isDone = false; }

private:
  std::function<void()> cbk;
  bool isDone = false;
};

#endif // GUIFROMIMGUI_H
