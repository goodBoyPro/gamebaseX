#ifndef GAMESTATICS_H
#define GAMESTATICS_H
#include <base/base.h>
class TextDrawer;
class GameConfig;
namespace GameStatics {
sf::ContextSettings &getWindowContexSettings();
GameClock &getGameClcok();

TextDrawer &getTextDrawer();
GameConfig &getGameConfig();
}; // namespace GameStatics
namespace GS = GameStatics;
#endif // GAMESTATICS_H