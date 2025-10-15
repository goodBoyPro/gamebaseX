#ifndef ASSETBROWSER_H
#define ASSETBROWSER_H
#include "gui/imguiDx/d11TextureAtlas.h"
#include "gui/imguiDx/editorUiSystem.h"

class AssetBrowser : public PanelNoResizeMove {
public:
  int selectedIndex = -1;
  ImVec2 itemSize = {50, 50};
  float distance = 15;
  ImVec2 topLeft = {10, 10};
  int columnCount = 5;

public:
  AssetBrowser(const std::string &id, int state)
      : PanelNoResizeMove(id, state) {
    setWindowUi([this]() {
      // lambda
      if (ImGui::BeginChild("AssetBrowserChild", ImVec2(0, 0), true)) {
        columnCount = ImGui::GetWindowWidth() / (distance + itemSize.x);
        for (int i = 0; i < 50; i++) {
          ImVec2 pos = getItemPos(i);
          ImGui::SetCursorPos(pos);
          ImGui::PushID(i);
          bool isSelected = (i == selectedIndex);
          if (ImGui::Selectable("", isSelected, 0, itemSize)) {
            selectedIndex = i;
          };
          ImGui::SetCursorPos(pos);
          //   ImGui::Image(icon1, {itemSize.x/2,itemSize.y/2});
          RenderAtlasImage("system/texture/a.png", pos, {30, 30});
          ImGui::SetCursorPos({pos.x, pos.y + itemSize.y - 10});
          if (isSelected) {
            ImGui::TextColored({255, 0, 255, 255}, "%d", i);
          } else {
            ImGui::Text("%d", i);
          }

          ImGui::PopID();
        }
      }
       ImGui::EndChild();
      // lambda
    });
  }
  ImVec2 getItemPos(int index) {
    return {index % columnCount * (itemSize.x + distance) + topLeft.x,
            (int)(index / columnCount) * (itemSize.y + distance) + topLeft.y};
  }
};
#endif // ASSETBROWSER_H