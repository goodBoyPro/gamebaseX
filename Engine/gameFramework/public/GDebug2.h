#ifndef GDEBUG2_H
#define GDEBUG2_H
#include "gameStatics.h"

#include <base/textDrawer.h>
#include <mutex>
#include <vector>
#define GDEBUG_STRINGSIZE 256
class GDebug2 {
private:
  std::vector<std::string> texts;
  void debugDisplay(class GameWindow &window_);
  std::mutex mut;

public:
  friend class GameWindow;
  GDebug2() = default;
  virtual ~GDebug2() = default;
  void print(std::string text);
};
#endif // GDEBUG2_H