#include "editorUiSystem.h"
UIManager::UIManager() { initImguiEnv(); }
BigWindow *UIManager::createBigWindow(const wchar_t *windowName,
                                      bool isMainWindow_) {

  BigWindow *ret = createBigWindowImgui(windowName, isMainWindow_);
  allWindows.push_back(ret);

  return ret;
}
UIManager::~UIManager() {
  for (BigWindow *window : allWindows) {
    delete window;
  }
  cleanImguiEnv();
}
void UIManager::MainLoop() {
  // for (BigWindow *window : allWindows) {
  //   window->loop();
  // }
  auto it =
      std::remove_if(allWindows.begin(), allWindows.end(), [&](BigWindow *w) {
        if (!(w->bValid)) {
          delete w;
          return true;
        } else {
          w->loop();
          return false;
        }
      });
  allWindows.erase(it, allWindows.end());
}
// 强制初始化d3d环境
struct UIManagerInitStruct {
  UIManagerInitStruct() { getUiManager(); }
};
static UIManagerInitStruct UIManagerInitStructInst;