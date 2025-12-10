#ifndef GDEBUG2_H
#define GDEBUG2_H
#include "gameStatics.h"
#include <base/gameWindow.h>
#include <base/textDrawer.h>
#include <mutex>
#include <vector>
#define GDEBUG_STRINGSIZE 256
class GDebug2 {
private:
  std::vector<std::string> texts;
  void debugDisplay(GameWindow &window_) {
    std::lock_guard<std::mutex> lock(mut);
    int index = 0;
    for (auto &a : texts) {
      GameStatics::getTextDrawer().printText(window_, a, 0, 30 * index);
      index++;
    }
    texts.clear();
  }
  std::mutex mut;

public:
  friend class GameWindow;
  GDebug2() = default;
  virtual ~GDebug2() = default;
  void printDebug(std::string text) {
    std::unique_lock lk(mut);
    texts.push_back(std::move(text));
  }
};
#endif // GDEBUG2_H