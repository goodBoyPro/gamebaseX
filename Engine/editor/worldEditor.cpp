#include "worldEditor.h"
void WorldEditorWindow::setMenus() {

    menus->setWindowUi([this]() {  
      menus->setSize({(float)(window.getSize().x),24});
      if (ImGui::BeginMenu("文件")) {
        if (ImGui::MenuItem("打开","ctrl+o")) {
          printf("open\n");
        }
        if (ImGui::MenuItem("保存","ctrl+s")) {
          printf("open\n");
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
            system("game.exe");
          }
        ImGui::EndMenu();
      }
    });
  }