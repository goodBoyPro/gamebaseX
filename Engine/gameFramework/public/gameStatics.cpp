#include "gameStatics.h"
#include <base/textDrawer.h>
static TextDrawer textDrawer;
TextDrawer& GameStatics::getTextDrawer() { return textDrawer; }
GameClock &GameStatics::getGameClcok() {
  static GameClock clock;
  return clock;
}
sf::ContextSettings &GameStatics::getWindowContexSettings() {
  static sf::ContextSettings settings;
  return settings;
}
