#ifndef BROWSErTEST_H
#define BROWSErTEST_H
#include "gui/imguiDx/imguiLib/imgui.h"
#include "stdio.h"
class BrowserTest {
public:
  ImVec2 itemSize = {50, 50};
  float distance = 15;
  ImVec2 topLeft = {10, 10};
  int columnCount = 5;
  ImVec2 getItemPos(int index) {
    return {index % columnCount * (itemSize.x + distance) + topLeft.x,
            (int)(index / columnCount) * (itemSize.y + distance) + topLeft.y};
  }
  void draw() {

    if (ImGui::Begin("testW")) {
      ImGui::Text("test1");
      ImGui::Separator();
      if (ImGui::BeginChild("child")) {
        columnCount=ImGui::GetWindowWidth()/(distance+itemSize.x);
        for (int i = 0; i < 50; i++) {
          ImVec2 pos=getItemPos(i);
          ImGui::SetCursorPos(pos);
          ImGui::PushID(i);
          ImGui::Selectable("", false, 0, itemSize);
          ImGui::SetCursorPos({pos.x,pos.y+itemSize.y-10});
          ImGui::Text("%d",i);
          ImGui::PopID();
        }

        ImGui::EndChild();
      }
      ImGui::End();
    }
  };
};
#endif //  BROWSErTEST_H