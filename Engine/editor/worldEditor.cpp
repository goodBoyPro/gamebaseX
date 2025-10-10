#include "worldEditor.h"
#include "materialEditPanel.h"
#include"fileManager.h"
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
        windowPop->setWindowUi([this]() {
          ImGui::Text("选择地图");
          auto &filesMap = FileManager::getFileManager().filesGmap;
          if (filesMap.size() == 0) {
            ImGui::Text("无可用地图");
            return;}
          static int mapIndex=0;
          if(ImGui::BeginCombo("地图",filesMap[mapIndex].name.c_str())){
            for(size_t i=0;i<filesMap.size();i++){
              if(ImGui::Selectable(filesMap[i].path.c_str(),mapIndex==i)){
                mapIndex=i;               
              }
            }
            ImGui::EndCombo();
          }

          if (ImGui::Button("确定")) {
           
            load(filesMap[mapIndex].path);
            windowPop->close();
          };
          ImGui::SameLine();
          if (ImGui::Button("取消")) {
            windowPop->close();
          };
        });
        windowPop->open();
      }
     
      if (ImGui::MenuItem("新建地图")) {
        static float mapInfo[4] = {0};
        static char path[128]={0};
        windowPop->setWindowUi([this]() {
          ImGui::InputText("路径", path, sizeof(path));
          ImGui::InputFloat("行", mapInfo);
          ImGui::InputFloat("列", mapInfo + 1);
          ImGui::InputFloat("单元宽", mapInfo + 2);
          ImGui::InputFloat("单元高", mapInfo + 3);
          auto&filesMat=FileManager::getFileManager().filesGmat;
          static int matIndex=0;
          if(ImGui::BeginCombo("地形材质",filesMat[matIndex].name.c_str())){
            for(size_t i=0;i<filesMat.size();i++){
              if(ImGui::Selectable(filesMat[i].path.c_str(),matIndex==i)){
                matIndex=i;               
              }
            }
            ImGui::EndCombo();
          }

          if (ImGui::Button("确定")) {
            if(path[0]==0)return;
            create(mapInfo[0], mapInfo[1], mapInfo[2], mapInfo[3],
                   filesMat[matIndex].path);
            
            worldFilePath=setExpand(path, ".gmap");
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
  //////////////////////////////////////////////////////////////////////////////////
  MiniWindow *landScapeMaterial =
      UI->createWindow<PanelNoResizeMove>("地形材质", 0);
  UI->layout.areaRight.addWindow(landScapeMaterial);
  landScapeMaterial->setWindowUi([&]() {
    if(curWorld==&waitPage){return;}
    materialEditPanel(curWorld->landScape.getMaterial());
    ImGui::Separator();
    float pixSize=window.getCameraActve()->getPixSize();//这里不能用static
    if (ImGui::DragFloat("视口缩放", &pixSize,0.0001,0.001,100,"%.4f")) {
      window.getCameraActve()->setPixSize(pixSize);
    }
  });
  //////////////////////////////////////////////////////////////////////////////////
  MiniWindow *win1 = UI->createWindow<PanelNoResizeMove>("window1", 0);
  UI->layout.areaLeft.addWindow(win1);
  win1->setPosition({10, 30});
  win1->setSize({300, 500});
  win1->setWindowUi([&]() {
    ////////////////////////////////////////////////////////向地图添加staticActor
    auto &names = ClassInfo::getStaticActors();
    static float pos[3] = {0};
    static float size[3] = {1, 1, 1};
    static std::string nameIndex = names.begin()->first;
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
   
    ImGui::InputText("名称##regSA", name, sizeof name);
    auto &files=FileManager::getFileManager().filesPng;
    static int curIndex=0;
    if(ImGui::BeginCombo("序列图", files[curIndex].name.c_str())){
      for(size_t i=0;i<files.size();i++){
        if(ImGui::Selectable(files[i].path.c_str(),curIndex==i)){
          curIndex = i;       
        }
      }
      ImGui::EndCombo();

    }
    ImGui::InputInt("index", &texIndex);
    if (ImGui::Button("注册")) {
      printf("register\n");
      ClassInfo::registerStaticActors(name,files[curIndex].path, texIndex);
    }
  });
  ////////////////////////////////////////////////地图视口
  MiniWindow *port = UI->createWindow<PortCarrierWindow>("port");
  ((PortCarrierWindow*)port)->otherHwnd=window.getSystemHandle();
  UI->layout.areaCenter.addWindow(port);


  ///////////////////////////////////////////////checker视口
  MiniWindow *checkerPort = UI->createWindow<PortCarrierWindow>("CheckerPort");
  UI->addOtherWindow(actorChecker.window.getSystemHandle());
  ((PortCarrierWindow *)checkerPort)->otherHwnd =
      actorChecker.window.getSystemHandle();
  UI->layout.areaBottomRgiht.addWindow(checkerPort);
  //////////////////////////////////////////////////////////////////////////
  MiniWindow*windowBottom=UI->createWindow<PanelNoResizeMove>("对象浏览",0);
  UI->layout.areaBottom.addWindow(windowBottom);
  ///////////////////////////////////////////////////执行布局
  UI->layout.match(UI->hwndMainWindow);
};
void WorldEditorWindow::loop() {

  if (window.isOpen()) {
    curWorld->loop(window, event);
    actorChecker.loop();
  } else {
    actorChecker.window.close();
    }
}
