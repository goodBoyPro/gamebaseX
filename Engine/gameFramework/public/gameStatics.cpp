#include "gameStatics.h"
#include <base/textDrawer.h>
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
// 强制全局对象初始化,确保初始化顺序正确//
static struct ____GlobalObjectInit {
  ____GlobalObjectInit() {
    GS::getTextDrawer();
    GameStatics::getGameClcok();
    GameStatics::getWindowContexSettings();
  }
} __globalObjectInit;
