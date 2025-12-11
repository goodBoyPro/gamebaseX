#include "gameStatics.h"
#include <base/textDrawer.h>
#include <gameConfig.h>
//////////

TextDrawer &GameStatics::getTextDrawer() {
  static TextDrawer textDrawer;
  return textDrawer;
}
/////////
GameClock &GameStatics::getGameClock() {
  static GameClock clock;
  return clock;
}
/////////
sf::ContextSettings &GameStatics::getWindowContextSettings() {
  static sf::ContextSettings settings;
  return settings;
}
////////
GameConfig &GameStatics::getGameConfig() {
  static GameConfig gameConfig;
  return gameConfig;
}
// 强制全局对象初始化,确保初始化顺序正确//
static struct GlobalObjectInit {
  GlobalObjectInit() {
    GS::getGameConfig();
    GS::getTextDrawer();
    GameStatics::getGameClock();
    GameStatics::getWindowContextSettings();
  }
} globalObjectInit;
