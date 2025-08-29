#include "worldEditor.h"
void WorldEditorWindow::setUI() {
  MiniWindow *menu = UI->createWindow<WindowMenu>(
      "menus", ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar |
                   ImGuiWindowFlags_NoResize);
  menu->setWindowUi([&]() {
    if (ImGui::BeginMenu("文件")) {
      if (ImGui::MenuItem("打开")) {
      }
      if (ImGui::MenuItem("保存")) {
      }
      if (ImGui::MenuItem("另存为")) {
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("编辑")) {
      if (ImGui::MenuItem("后退")) {
      }

      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("运行")) {
      if (ImGui::MenuItem("启动")) {
        
        
        std::thread th(&WorldEditorWindow::runGame, this);
        th.detach();
      }

      ImGui::EndMenu();
    }
  });
  
  MiniWindow *win1 = UI->createWindow<PanelNoResize>("window1",0);
  win1->setPosition({10, 30});
  win1->setSize({300, 500});
  win1->setWindowUi([]() {
    float c[4]={1,1,1,1};
    ImGui::InputFloat4("颜色", c, "%.4f", 1);
    ImVec4 color;
    ImGui::ColorButton("ys", color);
    ImGui::ColorEdit4("ys2", c);
  });
  
};