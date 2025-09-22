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
#include"miniWindow.h"
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

class BigWindow {
  
  
  ImColor clearColor = {0, 0, 0, 0};

public:
  WindowLayOut layout;
HWND hWnd = nullptr;
HWND parentHwnd = nullptr;
HWND otherHwnd=nullptr;
ImGuiContext *ctx = nullptr;
WindowPrivateData *data = nullptr;
  bool isMainWindow = false;
  bool bValid=true;
  bool brun=true;
  HWND hwndMainWindow=nullptr;
  friend class UIManager;
  void addOtherWindow(HWND other_) {
    otherHwnd=other_;
    SetParent(other_, hwndMainWindow);
    SetWindowPos(other_, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
  }
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
  void matchOther() {
    if (!otherHwnd)
      {return;}
      ImGuiStyle& style = ImGui::GetStyle();
      ImVec2 originalPadding = style.WindowPadding;
      float originalBorderSize = style.WindowBorderSize;
      
      // 临时移除内边距和边框
      style.WindowPadding = ImVec2(0, 0);
      style.WindowBorderSize = 0.0f;
    ImGui::SetNextWindowBgAlpha(0);
    
    ImGui::Begin("地图视口",nullptr,ImGuiWindowFlags_NoScrollbar);
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 size = ImGui::GetContentRegionAvail();
    MoveWindow(otherHwnd, pos.x,pos.y,size.x,size.y, true);
    ImGui::End();
    style.WindowPadding = originalPadding;
    style.WindowBorderSize = originalBorderSize;
  }
  void loop();
  BigWindow();
  ~BigWindow();

public:
  std::map<std::string, class MiniWindow *> allWindows;
  template <class T>
  MiniWindow *createWindow(const std::string &id, int state = 0);
};


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
void initImguiEnv();
void cleanImguiEnv();
BigWindow *createBigWindowImgui(const wchar_t *windowName,
                           bool isMainWindow_ = false);

#endif // GUIDX_H