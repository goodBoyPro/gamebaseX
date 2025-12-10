#include"GDebug2.h"
#include <base/gameWindow.h>

void GDebug2::debugDisplay(GameWindow &window_) {
  std::lock_guard<std::mutex> lock(mut);
  int index = 0;
  for (auto &a : texts) {
    GameStatics::getTextDrawer().printText(window_, a, 0, 30 * index);
    index++;
    
  }
  texts.clear();
}
