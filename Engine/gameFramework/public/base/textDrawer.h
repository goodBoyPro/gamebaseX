#ifndef TEXTDRAWER_H
#define TEXTDRAWER_H
#include "base.h"
#include <SFML/Graphics.hpp>
#include <cstdarg>
#include <vector>

class TextDrawer {
public:
  enum class EFontName : int {
    heiti = 0, //
    kaiti,
    zhunyuan,
    count
  };
  std::vector<std::string> fontPaths = {
      "res/font/heiti.ttf", "res/font/kaiti.ttf", "res/font/zhunyuan.ttf"};

private:
  sf::Font fonts[static_cast<int>(EFontName::count)];
  bool loadFonts();

public:
  TextDrawer() { loadFonts(); }

public:
  void printTextFormat(WindowBase &window_, int x, int y, int size,
                       ColorBase color, EFontName fontId, const wchar_t *format,
                       ...);
  void printTextW(WindowBase &window_, const std::wstring &str = L"未输入文字",
                  int x = 0, int y = 0, int size = 30,
                  sf::Color color = ColorBase(255, 255, 255),
                  EFontName fontId = EFontName::heiti);
  void printTextLimit(WindowBase &window_,
                      const std::wstring &str = L"未输入文字", float x = 0,
                      float y = 0, int size = 30,
                      const FVector2 &sprSize = {1, 1},
                      const ColorBase &color = ColorBase(255, 255, 255),
                      EFontName fontId = EFontName::heiti);
  void printNum(WindowBase &window_, float __float, int x = 0, int y = 0,
                int size = 30, sf::Color color = ColorBase(255, 255, 255),
                EFontName fontId = EFontName::heiti);

  void printNum(WindowBase &window_, int __int, int x = 0, int y = 0,
                int size = 30, sf::Color color = ColorBase(255, 255, 255),
                EFontName fontId = EFontName::heiti);
  void printText(WindowBase &window_, const std::string &str, int x = 0,
                 int y = 0, int size = 30,
                 sf::Color color = ColorBase(255, 255, 255),
                 EFontName fontId = EFontName::heiti,
                 ColorBase outlineColor = {0, 0, 0},
                 float outlineThickness = 0);
};
#endif // TEXTDRAWER_H