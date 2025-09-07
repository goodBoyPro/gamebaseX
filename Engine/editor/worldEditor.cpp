#include "worldEditor.h"

static std::function<void()> popCbk;
PopUpWindow *windowPop;
void WorldEditorWindow::setUI() {
  windowPop = (PopUpWindow *)(UI->createWindow<PopUpWindow>("popwindow"));

  MiniWindow *menu = UI->createWindow<WindowMenu>("menus");
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
        static float mapInfo[4] = {0};
        static char mapMatPath[64];
        windowPop->setWindowUi([this]() {
          ImGui::InputFloat("行", mapInfo);
          ImGui::InputFloat("列", mapInfo + 1);
          ImGui::InputFloat("单元宽", mapInfo + 2);
          ImGui::InputFloat("单元高", mapInfo + 3);
          ImGui::InputText("地形材质", mapMatPath, sizeof mapMatPath);
          if (ImGui::Button("确定")) {
            create(mapInfo[0], mapInfo[1], mapInfo[2], mapInfo[3], mapMatPath);
            windowPop->close();
          };
          ImGui::SameLine();
          if (ImGui::Button("取消")) {
            windowPop->close();
          };
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

        runGame();
      }

      ImGui::EndMenu();
    }
  });

  MiniWindow *win1 = UI->createWindow<PanelNoResize>("window1", 0);
  win1->setPosition({10, 30});
  win1->setSize({300, 500});
  win1->setWindowUi([&]() {
    ////////////////////////////////////////////////////////向地图添加staticActor
    auto &names = ClassInfo::getStaticActors();
    static float pos[3] = {0};
    static float size[3] = {1, 1, 1};
    static std::string nameIndex =names.begin()->first;
    if (ImGui::BeginCombo("名称", nameIndex.c_str())) {
      for (auto &n : names) {
        if (ImGui::Selectable(n.first.c_str(), nameIndex == n.first.c_str())) {
          nameIndex = n.first;
        }
      }

      ImGui::EndCombo();
    }
    
    ImGui::InputFloat3("位置", pos);
    ImGui::InputFloat3("大小", size);
    if (ImGui::Button("创建")) {
      curWorld->createStaticActor(nameIndex, {pos[0], pos[1], pos[2]},
                                  {size[0], size[1], size[2]});
    }
    ////////////////////////////////////////////////////////注册staticActor
    static char name[64] = {0};
    static int texIndex = 0;
    static char texPath[64] = {0};
    ImGui::InputText("名称##regSA", name, sizeof name);
    ImGui::InputText("序列图路径", texPath, sizeof texPath);
    ImGui::InputInt("index", &texIndex);
    if (ImGui::Button("注册")) {
      printf("register\n");
      ClassInfo::registerStaticActors(name, texPath, texIndex);
    }
  });
};
void WorldEditorWindow::loop() {

  if (window.isOpen()) {
    curWorld->loop(window, event);
  }
}
