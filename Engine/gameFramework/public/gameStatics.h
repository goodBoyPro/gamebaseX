#ifndef GAMESTATICS_H
#define GAMESTATICS_H
#include <base/base.h>
namespace GameStatics {
inline sf::ContextSettings &getWindowContexSettings() {
  static sf::ContextSettings settings;
  return settings;
}
inline GameClock &getGameClcok() {
  static GameClock clock;
  return clock;
}
} // namespace GameStatics
#endif // GAMESTATICS_H