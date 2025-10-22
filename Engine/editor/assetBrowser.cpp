#include "assetBrowser.h"
#include "worldEditor.h"
AssetBrowser::AssetBrowser(const std::string &id, int state)
    : PanelNoResizeMove(id, state) {
  setWindowUi([this]() {
    // lambda
    // if (ImGui::BeginChild("AssetBrowserChild", ImVec2(0, 0), true)) {
    //   columnCount = ImGui::GetWindowWidth() / (distance + itemSize.x);
    //   for (int i = 0; i < 50; i++) {
    //     ImVec2 pos = getItemPos(i);
    //     ImGui::SetCursorPos(pos);
    //     ImGui::PushID(i);
    //     bool isSelected = (i == selectedIndex);
    //     if (ImGui::Selectable("", isSelected, 0, itemSize)) {
    //       selectedIndex = i;
    //     };
    //     ImGui::SetCursorPos(pos);
    //     RenderAtlasImage("system/texture/a.png", pos, {30, 30});
    //     ImGui::SetCursorPos({pos.x, pos.y + itemSize.y - 10});
    //     if (isSelected) {
    //       ImGui::TextColored({255, 0, 255, 255}, "%d", i);
    //     } else {
    //       ImGui::Text("%d", i);
    //     }

    //     ImGui::PopID();
    //   }
    // }
    //  ImGui::EndChild();
    if (ImGui::BeginChild("AssetBrowserChild", ImVec2(0, 0), true)) {
      columnCount = ImGui::GetWindowWidth() / (distance + itemSize.x);
      int i = 0;
      for (ActorInfoIf *infoptr : ActorInfoIf::allInfos) {
        ImVec2 pos = getItemPos(i);
        ImGui::SetCursorPos(pos);
        ImGui::PushID(i);
        bool isSelected = (i == selectedIndex);
        if (ImGui::Selectable("", isSelected, 0, itemSize)) {
          selectedIndex = i;
          if (selectedActor) {
            selectedActor->destroyActor();
          }
          selectedActor = infoptr->createInWorld(mainWindow->actorChecker.curWorld);
        };
        ImGui::SetCursorPos(pos);
        RenderAtlasImage("system/texture/a.png", pos, {30, 30});
        ImGui::SetCursorPos({pos.x, pos.y + itemSize.y - 10});
        if (isSelected) {
          ImGui::TextColored({255, 0, 255, 255}, "%s", infoptr->name.c_str());
        } else {
          ImGui::Text("%s", infoptr->name.c_str());
        }

        ImGui::PopID();
        i++;
      }
    }
    ImGui::EndChild();
    // lambda
  });
}
