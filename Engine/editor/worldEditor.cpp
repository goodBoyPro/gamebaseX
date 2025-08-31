#include "worldEditor.h"
static char name[64];
static float pos[3] = {0};
static float size[3]={1,1,1};
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
  win1->setWindowUi([&]() {
    ImGui::InputText("name", name, sizeof name);
    ImGui::InputFloat3("位置", pos);
    ImGui::InputFloat3("大小", size);
    if (ImGui::Button("创建")) {
      curWorld->createStaticActor(name,{pos[0],pos[1],pos[2]},{size[0],size[1],size[2]});
    }
  });
  
};