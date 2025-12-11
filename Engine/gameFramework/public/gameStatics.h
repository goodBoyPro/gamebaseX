#ifndef GAMESTATICS_H
#define GAMESTATICS_H
#include <base/base.h>
class TextDrawer;
class GameConfig;
namespace GameStatics {
sf::ContextSettings &getWindowContextSettings();
GameClock &getGameClock();
TextDrawer &getTextDrawer();
GameConfig &getGameConfig();
}; // namespace GameStatics
namespace GS = GameStatics;
#endif // GAMESTATICS_H