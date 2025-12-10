#ifndef GAMESTATICS_H
#define GAMESTATICS_H
#include <base/base.h>
class TextDrawer;
namespace GameStatics {
sf::ContextSettings &getWindowContexSettings();
GameClock &getGameClcok();

TextDrawer &getTextDrawer();
}; // namespace GameStatics
namespace GS = GameStatics;
#endif // GAMESTATICS_H