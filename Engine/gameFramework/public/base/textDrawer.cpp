#include "textDrawer.h"

bool TextDrawer::loadFonts() {
  int count = static_cast<int>(EFontName::count);
  bool ret = true;
  for (int i = 0; i < count; i++) {
    bool result = fonts[i].loadFromFile(fontPaths[i]);
    if (!result) {
      printf("FAILED LOAD FONT:%s", fontPaths[i].c_str());
      ret = false;
    }
  }
  return ret;
}
void TextDrawer::printTextFormat(WindowBase &window_, int x, int y, int size,
                                 ColorBase color, EFontName fontId,
                                 const wchar_t *format, ...) {
  wchar_t str[1024];
  va_list args;
  va_start(args, format);
  vswprintf_s(str, format, args);
  va_end(args);
  printTextW(window_, str, x, y, size, color, fontId);
}
void TextDrawer::printTextW(WindowBase &window_, const std::wstring &str, int x,
                            int y, int size, sf::Color color,
                            EFontName fontId) {
  thread_local static sf::Text text;
  text.setFont(fonts[static_cast<int>(fontId)]);
  text.setString(str);
  text.setPosition(sf::Vector2f(x, y));
  text.setFillColor(color);
  text.setCharacterSize(size);
  window_.draw(text);
}
void TextDrawer::printTextLimit(WindowBase &window_, const std::wstring &str,
                                float x, float y, int size,
                                const FVector2 &sprSize, const ColorBase &color,
                                EFontName fontId) {
  thread_local static sf::Text text;
  text.setFont(fonts[static_cast<int>(fontId)]);
  text.setString(str);
  text.setPosition(x, y);
  text.setFillColor(color);
  text.setCharacterSize(size);
  FVector2 scale = {sprSize.x / text.getLocalBounds().getSize().x,
                    sprSize.y / text.getLocalBounds().getSize().y};
  text.setScale(scale);
  window_.draw(text);
}
void TextDrawer::printNum(WindowBase &window_, float __float, int x, int y,
                          int size, sf::Color color, EFontName fontId) {
  wchar_t num[32];
  swprintf_s(num, L"%f", __float);
  printTextW(window_, num, x, y, size, color, fontId);
}
void TextDrawer::printNum(WindowBase &window_, int __int, int x, int y,
                          int size, sf::Color color, EFontName fontId) {
  wchar_t num[32];
  swprintf_s(num, L"%d", __int);
  printTextW(window_, num, x, y, size, color, fontId);
}

void TextDrawer::printText(WindowBase &window_, const std::string &str, int x,
                           int y, int size, sf::Color color, EFontName fontId,
                           ColorBase outlineColor, float outlineThickness) {
  thread_local static sf::Text text;
  text.setFont(fonts[static_cast<int>(fontId)]);
  text.setString(sf::String::fromUtf8(str.begin(), str.end()));
  text.setPosition(sf::Vector2f(x, y));
  text.setFillColor(color);
  text.setOutlineColor(outlineColor);
  text.setOutlineThickness(outlineThickness);
  text.setCharacterSize(size);
  window_.draw(text);
}
