#ifndef GAMESTATICS_H
#define GAMESTATICS_H
#include <base/base.h>
namespace GameStatics {
inline GameClock &getGameClcok() {
  static GameClock clock;
  return clock;
}
} // namespace GameStatics
#endif // GAMESTATICS_H