#ifndef ASSETBROWSER_H
#define ASSETBROWSER_H
#include "gui/imguiDx/d11TextureAtlas.h"
#include "gui/imguiDx/editorUiSystem.h"
#include"base/registeredInfo.h"
class AssetBrowser : public PanelNoResizeMove {
public:
  int selectedIndex = -1;
  ImVec2 itemSize = {50, 50};
  float distance = 15;
  ImVec2 topLeft = {10, 10};
  int columnCount = 5;
  class WorldEditorWindow *mainWindow;
  GActor*selectedActor=nullptr;
public:
  AssetBrowser(const std::string &id, int state);
  ImVec2 getItemPos(int index) {
    return {index % columnCount * (itemSize.x + distance) + topLeft.x,
            (int)(index / columnCount) * (itemSize.y + distance) + topLeft.y};
  }
};
#endif // ASSETBROWSER_H