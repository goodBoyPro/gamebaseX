#include"GDebug2.h"
#include <base/gameWindow.h>

void GDebug2::debugDisplay(GameWindow &window_) {
  std::lock_guard<std::mutex> lock(mut);
  int index = 0;
  for (auto &a : texts) {
    GameStatics::getTextDrawer().printText(window_, a, 0, 30 * index,30,{255,255,255},
                                          TextDrawer::EFontName::heiti,{0,0,0},3);
    index++;
    
  }
  texts.clear();
}
void GDebug2::print(std::string text) {
  std::unique_lock lk(mut);
  texts.push_back(std::move(text));
}
