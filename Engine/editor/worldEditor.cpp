#include "worldEditor.h"
static char name[64];
static float pos[3] = {0};
static float size[3] = {1, 1, 1};
static float mapInfo[4] = {0};
static char mapMatPath[64];
static char mapPath[64];
static std::function<void()>popCbk;
PopUpWindow *windowPop;
void WorldEditorWindow::setUI() {
  windowPop =
      (PopUpWindow *)(UI->createWindow<PopUpWindow>("popwindow"));
 
  MiniWindow *menu = UI->createWindow<WindowMenu>(
      "menus");
  menu->setWindowUi([&]() {
   
    if (ImGui::BeginMenu("文件")) {
      if (ImGui::MenuItem("打开")) {
      }
      if (ImGui::MenuItem("保存")) {
        save();
      }
      if (ImGui::MenuItem("另存为")) {
      }
      if (ImGui::MenuItem("加载地图")) {
        
      }
      if (ImGui::MenuItem("新建地图")) {

        windowPop->setWindowUi([this]() {
          ImGui::InputText("路径", mapPath, sizeof mapPath);
          ImGui::InputFloat("行", mapInfo);
          ImGui::InputFloat("列", mapInfo+1);
          ImGui::InputFloat("单元宽", mapInfo+2);
          ImGui::InputFloat("单元高", mapInfo + 3);
          ImGui::InputText("材质", mapMatPath, sizeof mapMatPath);
          if(ImGui::Button("确定")){create(mapPath, mapInfo[0],mapInfo[1],mapInfo[2],mapInfo[3],mapMatPath);windowPop->close();};
          if(ImGui::Button("取消")){windowPop->close();};
        });
        windowPop->open();
        
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