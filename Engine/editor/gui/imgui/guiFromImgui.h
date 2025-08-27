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

class ImguiManager {
  std::vector<class guiFromImgui *> allBigWindow;
  ImguiManager() {
  
  }

public:
  inline static ImguiManager &getUI() {
    static ImguiManager ret;
    return ret;
  }
  guiFromImgui *createBigWindow(const wchar_t *windowName,HWND parent_);
  ~ImguiManager() {
    for (guiFromImgui *g : allBigWindow) {
      delete g;
    }
  }
  void mainLoop();
};
class guiFromImgui {
public:
  struct DoOnce;
  HWND parent;
  ImGuiContext *windowContex;
  // static guiFromImgui &getUi();
  void init(const wchar_t *windowName=L"window");
  void close() { bOpen = false; };
  bool isOpen() { return bOpen; }
  // 在应用的主循环中调用这个方法
  void mainLoop();
  //////小窗口///////////////////////////////////////////////////////////////////////////////////////////////////
  std::map<std::string, class UiWindow *> allWindows;
  template <class T>
  UiWindow *createWindow(const std::string &id, int state = 0);

  ~guiFromImgui();
  guiFromImgui();
  void setFollowOtherWindow(HWND hwnd_) {
    SetLayeredWindowAttributes(hwnd,
                               ImColor(clear_color_with_alpha[0] * 255,
                                       clear_color_with_alpha[1] * 255,
                                       clear_color_with_alpha[2] * 255),
                               NULL, LWA_COLORKEY);
    SetParent(hwnd, hwnd_);
    SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
    
    
  }
  void followOtherWindow(HWND hwnd_) {
    RECT rect;
    GetClientRect(hwnd_, &rect);
    // POINT point = {rect.left, rect.top};
    // ClientToScreen(hwnd_, &point);
    // MoveWindow(hwnd, point.x, point.y, rect.right, rect.bottom, true);
    MoveWindow(hwnd, 0, 0, rect.right, rect.bottom, true);
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
  bool couldClose=true;
  protected:
  ImVec2 position = {100, 0};
  ImVec2 size={100,100};
public:
  UiWindow(const std::string &id_, int state_) {
    id = id_;
    state = state_;
    ImGuiStyle &style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Text]=ImVec4(0.1,0.1,0.1,1);
  };
  virtual ~UiWindow() { printf("uncons\n"); };
  std::function<void()> window = []() {};
 void disableClose(){couldClose=false;}
public:
  std::string id;
  bool bOpen = true;
  void setPosition(const ImVec2 &pos_) { position = pos_; }
  void setSize(const ImVec2&size_){size=size_;}
  int state = 0;
  // 这里分开写是因为需要在lamda表达式内部调用自身
  void setWindowUi(const std::function<void()> &callback) {
    window = callback;
  };
  virtual void loop() {
    if ((bOpen)) {
      if(couldClose){ImGui::Begin(id.c_str(), &bOpen, state);}else {
        ImGui::Begin(id.c_str(), nullptr, state);
      }
      
      window();
      ImGui::End();
    }
  }
};
///////////////////////////////////////////////////////////////////////////////////////////////////////
class Menus : public UiWindow {
public:
  Menus(const std::string &id_, int state_=ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize) : UiWindow(id_, state_) { setPosition({0,0});}
  void loop() override {
    if ((bOpen)) {
      ImGui::SetNextWindowPos(position);
      ImGui::SetNextWindowSize(size);
      ImGui::Begin(id.c_str(), nullptr, state|ImGuiWindowFlags_MenuBar);

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

template <class T>
inline UiWindow *guiFromImgui::createWindow(const std::string &id, int state) {
  if (allWindows.find(id) != allWindows.end()) {
    printf("error:window id redefined");
    assert(false);
  }
  UiWindow *window = new T(id, state);
  allWindows[id] = window;
  return window;
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
