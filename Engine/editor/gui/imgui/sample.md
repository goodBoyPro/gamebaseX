```c++
//编译时需要链接   d3d11 dwmapi d3dcompiler
#include <guiFromImgui.h>
int main() {
   guiFromImgui &ui = guiFromImgui::getUi();
   ImVec4 color(1, 0, 0, 1);
   float color2[4] = {0, 0, 0, 1};
   UiWindow &wind1 = ui.createWindow("测试窗口1", 0);
   wind1.setWindowUi([&]() {
      ImGui::ColorButton("color", color);
      ImGui::ColorEdit4("color2", color2);
      ImGui::BeginTabBar("beginTabBar");
      ImGui::EndTabBar();
      

      ImGui::BeginGroup();
      ImGui::Button("button2");
      ImGui::SameLine(200);
      ImGui::Button("按键测试1");
      ImGui::EndGroup();
      if (ImGui::IsItemHovered()) {
         
      }
      ImGui::BeginTable("table", 3, ImGuiTableFlags_Borders);
      ImGui::TableSetupColumn("c1");
      ImGui::TableSetupColumn("c2");
      ImGui::TableSetupColumn("c3");
      for (int row = 0; row < 10; row++) // 假设有10行数据
      {
         ImGui::TableNextRow();
         for (int col = 0; col < 3; col++) {
            ImGui::TableNextColumn();
            // 在这里填充单元格数据，例如 ImGui::Text(...)
            ImGui::Text("Data %d,%d", row, col);
         }
      }
      ImGui::EndTable();
   });
   while (ui.isOpen()) {
      ui.mainLoop();
   }

   return 0;
}
```