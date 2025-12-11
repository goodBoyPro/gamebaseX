#include "gameStatics.h"
#include <base/textDrawer.h>
#include <gameConfig.h>
//////////
static TextDrawer textDrawer;
TextDrawer &GameStatics::getTextDrawer() { return textDrawer; }
/////////
GameClock &GameStatics::getGameClcok() {
  static GameClock clock;
  return clock;
}
/////////
sf::ContextSettings &GameStatics::getWindowContexSettings() {
  static sf::ContextSettings settings;
  return settings;
}
////////
GameConfig &GameStatics::getGameConfig() { return GameConfig::getGameConfig(); }
// 强制全局对象初始化,确保初始化顺序正确//
static struct ____GlobalObjectInit {
  ____GlobalObjectInit() {
    GS::getGameConfig();
    GS::getTextDrawer();
    GameStatics::getGameClcok();
    GameStatics::getWindowContexSettings();
  }
} __globalObjectInit;
