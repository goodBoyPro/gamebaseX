#ifndef MATERIALEDITPANEL_H
#define MATERIALEDITPANEL_H
#include "base/registeredInfo.h"
#include "framework.h"
#include "gui/imguiDx/guiDx.h"
#include "render/sprite.h"

inline void materialEditPanel(GMaterial &mat) {
  for (auto &[k, v] : mat.properties) {
    ImGui::SetNextItemWidth(100.0f);
    if (ImGui::DragFloat(k.c_str(), &(v.value),0.01, v.min, v.max)) {
      mat.setValueScalarByname(k, v.value);
    }
  }
  ImGui::Separator();
  for (auto &[k, v] : mat.vec4Properties) {
    if (ImGui::DragFloat4(k.c_str(), v.data(),0.01f,-1.f,1.f)) {
      mat.setValueVectorByname(k, {v[0],v[1],v[2],v[3]});
    }
  }
  if (ImGui::Button("刷新")) {
    mat.reLoadMat();
  }
  ImGui::SameLine();
  if (ImGui::Button("保存##matLS")) {
    mat.save();
  }
}
#endif // MATERIALEDITPANEL_H
