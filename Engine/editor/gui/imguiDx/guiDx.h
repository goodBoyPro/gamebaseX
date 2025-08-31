#ifndef GUIDX_H
#define GUIDX_H

#include "functional"
#include "imguiLib/imgui.h"
#include "imguiLib/imgui_impl_dx11.h"
#include "imguiLib/imgui_impl_win32.h"
#include "map"
#include "stdexcept"
#include <d3d11.h>
#include <dxgi.h>
#include <stdio.h>
#include <tchar.h>
#include <vector>
#include <windows.h>

// 窗口私有资源结构体
struct WindowPrivateData {
  ImGuiContext *imguiContext;  // 窗口专属ImGui上下文
  ID3D11RenderTargetView *rtv; // 渲染目标视图
  IDXGISwapChain *swapChain;   // 交换链
  const WCHAR *className;      // 窗口类名
  int fontIndex;               // 当前使用的字体索引
};

// 字体信息结构体
struct FontInfo {
  const char *filePath;                // 字体文件路径
  float size;                          // 字体大小
  const char *name;                    // 字体名称
  ImFont *font;                        // 预加载后的字体对象（全局共享）
  std::vector<unsigned char> fontData; // 缓存字体文件数据（避免重复IO）
};
/////////////////////////////////////////////////////////////////////////////////////////////////////
class MiniWindow {
public:
  bool couldClose = true;
  

protected:
  ImVec2 position = {100, 0};
  ImVec2 size = {100, 100};

public:
  MiniWindow(const std::string &id_, int state_) {
    id = id_;
    state = state_;
    ImGuiStyle &style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Text] = ImVec4(0.1, 0.1, 0.1, 1);
  };
  virtual ~MiniWindow() { printf("uncons\n"); };
  std::function<void()> imguiRenderCBK = []() {};
  void disableClose() { couldClose = false; }

public:
  std::string id;
  bool bOpen = true;
  void setPosition(const ImVec2 &pos_) { position = pos_; }
  void setSize(const ImVec2 &size_) { size = size_; }
  int state = 0;
  // 这里分开写是因为需要在lamda表达式内部调用自身
  void setWindowUi(const std::function<void()> &callback) {
    imguiRenderCBK = callback;
  };
  virtual void loop() {
    if ((bOpen)) {
      if (couldClose) {
       
        ImGui::Begin(id.c_str(), &bOpen, state);
      } else {
        ImGui::Begin(id.c_str(), nullptr, state);
      }

      imguiRenderCBK();
      ImGui::End();
    }
  }
};
class BigWindow {
  HWND hWnd = nullptr;
  HWND parentHwnd = nullptr;
  ImGuiContext *ctx = nullptr;
  WindowPrivateData *data = nullptr;
  ImColor clearColor = {0, 0, 0, 0};

public:
  friend class UIManager;
  void setParentWindow(HWND parent_) {
    parentHwnd = parent_;
    SetLayeredWindowAttributes(hWnd, ImColor(0, 0, 0), NULL, LWA_COLORKEY);
    SetParent(hWnd, parent_);
    SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
  }
  void followParent() {
    if (parentHwnd) {
      RECT rect;
      GetClientRect(parentHwnd, &rect);
      MoveWindow(hWnd, 0, 0, rect.right, rect.bottom, true);
    }
  }
  void loop();
  BigWindow() {}
  ~BigWindow();

public:
  std::map<std::string, class MiniWindow *> allWindows;
  template <class T>
  MiniWindow *createWindow(const std::string &id, int state = 0);
};
class UIManager {
  const WCHAR *sharedClassName = L"ImGui_DX11_Shared_Window";
  UIManager();
  std::vector<BigWindow *> allWindows;

public:
  BigWindow *createBigWindow(const wchar_t *windowName, HWND parent_ = nullptr);
  void MainLoop();
  static UIManager &getUI() {
    static UIManager ret;
    return ret;
  }
  ~UIManager();
};
inline UIManager &getUiManager() { return UIManager::getUI(); }
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
template <class T>
inline MiniWindow *BigWindow::createWindow(const std::string &id, int state) {
  if (allWindows.find(id) != allWindows.end()) {
    printf("error:window id redefined");
    assert(false);
  }
  MiniWindow *window = new T(id, state);
  allWindows[id] = window;
  return window;
}
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
class WindowMenu : public MiniWindow {
public:
  WindowMenu(const std::string &id, int state) : MiniWindow(id, state) {}
  void loop() override {
    if (bOpen) {
      if (ImGui::BeginMainMenuBar()) {
        imguiRenderCBK();
        ImGui::EndMainMenuBar();
      }
    }
  }
};
class PanelNoResize : public MiniWindow {
  public:
  PanelNoResize(const std::string &id, int state) : MiniWindow(id, state) {}
  void loop() override {
    if (bOpen) {
      
      ImGui::SetNextWindowSize(size);
      if (ImGui::Begin(id.c_str(),nullptr,ImGuiWindowFlags_NoResize)) {
        imguiRenderCBK();
        ImGui::End();
      }
    }
  }
};

#endif // GUIDX_H