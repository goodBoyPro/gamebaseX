#ifndef EDITORUISYSTEM_H
#define EDITORUISYSTEM_H
#include "guidx.h"
class UIManager {
  // const WCHAR *sharedClassName = L"ImGui_DX11_Shared_Window";
  UIManager();
  std::vector<BigWindow *> allWindows;

public:
  BigWindow *createBigWindow(const wchar_t *windowName,
                             bool isMainWindow_ = false);
  void MainLoop();
  static UIManager &getUI() {
    static UIManager ret;
    return ret;
  }
  ~UIManager();
};
inline UIManager &getUiManager() { return UIManager::getUI(); }

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
      if (ImGui::Begin(id.c_str(), nullptr, ImGuiWindowFlags_NoResize)) {
        imguiRenderCBK();
       
      }
       ImGui::End();
    }
  }
};
class PanelNoResizeMove : public MiniWindow {
public:
  PanelNoResizeMove(const std::string &id, int state) : MiniWindow(id, state) {}
  void loop() override {
    if (bOpen) {

      ImGui::SetNextWindowSize(size);
      ImGui::SetNextWindowPos(position);
      if (ImGui::Begin(id.c_str(), nullptr,
                       ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
        imguiRenderCBK();
       
      }
       ImGui::End();
    }
  }
};
class PopUpWindow : public MiniWindow {
public:
  PopUpWindow(const std::string &id, int state) : MiniWindow(id, state) {}
  bool bPop = false;
  void open() { bPop = true; }
  void close() { bPop = false; }
  void loop() override {
    if (bPop) {
      ImGui::OpenPopup(id.c_str());
      if (ImGui::BeginPopupModal(id.c_str())) {
        imguiRenderCBK();
        ImGui::EndPopup();
      }
    }
  }
};
class CustomWindow : public MiniWindow {
public:
  CustomWindow(const std::string &id, int state) : MiniWindow(id, state) {}
  void loop() override {
    if (bOpen) {
      imguiRenderCBK();
    }
  }
};
class PortCarrierWindow : public MiniWindow {
public:
  PortCarrierWindow(const std::string &id, int state) : MiniWindow(id, state) {}
  HWND otherHwnd = nullptr;
  void loop() override {
    if (!otherHwnd) {
      return;
    }
    ImGuiStyle &style = ImGui::GetStyle();
    ImVec2 originalPadding = style.WindowPadding;
    float originalBorderSize = style.WindowBorderSize;

    // 临时移除内边距和边框
    style.WindowPadding = ImVec2(0, 0);
    style.WindowBorderSize = 0.0f;
    ImGui::SetNextWindowBgAlpha(0);
    ImGui::SetNextWindowSize(size);
    ImGui::SetNextWindowPos(position);
    if (ImGui::Begin(id.c_str(), nullptr,
                     ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoMove)) {
      ImVec2 pos = ImGui::GetCursorScreenPos();
      ImVec2 size = ImGui::GetContentRegionAvail();
      MoveWindow(otherHwnd, pos.x, pos.y, size.x, size.y, true);
      
    }
    ImGui::End();

    style.WindowPadding = originalPadding;
    style.WindowBorderSize = originalBorderSize;
  };
};
#endif