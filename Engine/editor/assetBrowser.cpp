// #include "assetBrowser.h"
// #include "worldEditor.h"
// AssetBrowser::AssetBrowser(const std::string &id, int state)
//     : PanelNoResizeMove(id, state) {
//   setWindowUi([this]() {
//     // lambda
//     if (ImGui::BeginChild("AssetBrowserChild", ImVec2(0, 0), true)) {
//       columnCount = ImGui::GetWindowWidth() / (distance + itemSize.x);
//       int i = 0;
//       for (ActorInfoIf *infoptr : ActorInfoIf::allInfos) {
//         ImVec2 pos = getItemPos(i);
//         ImGui::SetCursorPos(pos);
//         ImGui::PushID(i);
//         bool isSelected = (i == selectedIndex);
//         if (ImGui::Selectable("", isSelected, 0, itemSize)) {
//           selectedIndex = i;
//           if (selectedActor) {
//             selectedActor->destroyActor();
//           }
//           selectedActor =
//           infoptr->createInWorld(mainWindow->actorChecker.curWorld);
//           dynamic_cast<GStaticActor*>(selectedActor)->sprComp->setSizeWs({2,2,0.5});
//         };
//         ImGui::SetCursorPos(pos);
//         RenderAtlasImage("system/texture/a.png", pos, {30, 30});
//         ImGui::SetCursorPos({pos.x, pos.y + itemSize.y - 10});
//         if (isSelected) {
//           ImGui::TextColored({255, 0, 255, 255}, "%s",
//           infoptr->name.c_str());
//         } else {
//           ImGui::Text("%s", infoptr->name.c_str());
//         }

//         ImGui::PopID();
//         i++;
//       }
//     }
//     ImGui::EndChild();
//     // lambda
//   });
// }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "assetBrowser.h"
#include "worldEditor.h"

// 定义拖拽数据类型标识（全局唯一，用于区分不同拖拽类型）
AssetBrowser::AssetBrowser(const std::string &id, int state)
    : PanelNoResizeMove(id, state) {
  setWindowUi([this]() {
    if (ImGui::BeginChild("AssetBrowserChild", ImVec2(0, 0), true)) {
      // 原有计算列数逻辑
      columnCount = ImGui::GetWindowWidth() / (distance + itemSize.x);
      int i = 0;
      for (ActorInfoIf *infoptr : ActorInfoIf::allInfos) {
        ImVec2 pos = getItemPos(i);
        ImGui::SetCursorPos(pos);
        ImGui::PushID(i); // 保持原有ID管理

        bool isSelected = (i == selectedIndex);
        // 原有点击选中逻辑（完全保留）
        if (ImGui::Selectable("", isSelected, 0, itemSize)) {
          selectedIndex = i;
          if (selectedActor) {
            selectedActor->destroyActor();
          }
          selectedActor =
              infoptr->createInWorld(mainWindow->actorChecker.curWorld);
          dynamic_cast<GStaticActor *>(selectedActor)
              ->sprComp->setSizeWs

              ({2, 2, 0.5});
        }

        // 新增：拖拽源逻辑（不影响原有点击功能）
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
          // 传递当前资源指针作为拖拽数据
          ImGui::SetDragDropPayload(ASSET_BROWSER_DRAG_TYPE, &infoptr,
                                    sizeof(ActorInfoIf *));
          // 拖拽时显示的预览内容（可选，不影响核心功能）
          ImGui::Text("拖拽: %s", infoptr->name.c_str());
          RenderAtlasImage("system/texture/a.png", ImVec2(0, 20), {30, 30});

          ImGui::EndDragDropSource();
        }

        // 原有绘制图标逻辑（完全保留）
        ImGui::SetCursorPos(pos);
        RenderAtlasImage("system/texture/a.png", pos, {30, 30});

        // 原有绘制文字逻辑（修正颜色值范围为0.0f-1.0f，不改变视觉效果）
        ImGui::SetCursorPos({pos.x, pos.y + itemSize.y - 10});
        if (isSelected) {
          // 原255,0,255对应1.0f,0.0f,1.0f（紫色），保持视觉一致
          ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "%s",
                             infoptr->name.c_str());
        } else {
          ImGui::Text("%s", infoptr->name.c_str());
        }

        ImGui::PopID();
        i++;
      }
    }
    ImGui::EndChild();
  });
}

// 在目标窗口执行回调
//  if (ImGui::Begin("World Viewport")) {
//      // 视口原有绘制逻辑...

//     // 检测拖拽目标并处理回调
//     if (ImGui::BeginDragDropTarget()) {
//       if (const ImGuiPayload* payload =
//       ImGui::AcceptDragDropPayload(ASSET_BROWSER_DRAG_TYPE)) {
//         // 验证拖拽数据有效性
//         IM_ASSERT(payload->DataSize == sizeof(ActorInfoIf*));
//         ActorInfoIf* draggedAsset = *(ActorInfoIf**)payload->Data;

//         // 松开鼠标时执行的回调（可根据需求自定义）
//         if (draggedAsset) {
//           // 示例：创建拖拽的Actor（与点击选中逻辑一致，避免功能冲突）
//           if (selectedActor) {
//             selectedActor->destroyActor();
//           }
//           selectedActor = draggedAsset->createInWorld(actorChecker.curWorld);
//           if (auto staticActor = dynamic_cast<GStaticActor*>(selectedActor))
//           {
//             staticActor->sprComp->setSizeWs({2, 2, 0.5});
//           }
//         }
//       }
//       ImGui::EndDragDropTarget();
//     }
//   }
//   ImGui::End();
// }
