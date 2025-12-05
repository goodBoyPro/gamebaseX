#include "GComman.h"
#include <cstdarg>
sf::Font &getFont(int id) {
    static sf::Font font[5];
    static volatile bool f = []() {
      font[0].loadFromFile("res/font/heiti.ttf");
      font[1].loadFromFile("res/font/kaiti.ttf");
      font[2].loadFromFile("res/font/zhunyuan.ttf");
      return true;
    }();
    (void)f;
    return font[id];
    
}
struct __useGetFontAutoBeforMain {
  __useGetFontAutoBeforMain(){getFont(0);};
}__useGetFontinst;
void printTextFormat(GameWindow &window_, int x, int y, int size, ColorBase color, int fontId, const wchar_t *format, ...) {
    wchar_t str[1024];
    va_list args;
    va_start(args, format);
    vswprintf_s(str, format, args);
    va_end(args);
    printText(window_, str, x, y, size, color, fontId);
}
void printText(GameWindow &window_, const std::wstring &str, int x, int y, int size, ColorBase color, int fontId) {
    static sf::Text text;
    text.setFont(getFont(fontId));
    text.setString(str);
    text.setPosition(sf::Vector2f(x, y));
    text.setFillColor(color);
    text.setCharacterSize(size);
    window_.draw(text);
}
void printTextLimit(GameWindow &window_, const std::wstring &str, float x, float y, int size, const FVector2 &sprSize, const ColorBase &color, int fontId) {
    static sf::Text text;
    text.setFont(getFont(fontId));
    text.setString(str);
    text.setPosition(x, y);
    text.setFillColor(color);
    text.setCharacterSize(size);
    FVector2 scale = {sprSize.x / text.getLocalBounds().getSize().x, sprSize.y / text.getLocalBounds().getSize().y};
    text.setScale(scale);
    window_.draw(text);
}
void printNum(GameWindow &window_, float __float, int x, int y, int size, ColorBase color, int fontId) {
    wchar_t num[32];
    swprintf_s(num, L"%f", __float);
    printText(window_, num, x, y, size, color, fontId);
}

void printNum(GameWindow &window_, int __int, int x, int y, int size, ColorBase color, int fontId) {
    wchar_t num[32];
    swprintf_s(num, L"%d", __int);
    printText(window_, num, x, y, size, color, fontId);
}

FVector3 vectorNormalize(const FVector3 &vec) {
    float len = getVectorLen(vec);
    if (len < 0.00001)
        return {0, 0, 0};
    return vec / len;
}
float getVectorLen(const FVector3 &vec) { return std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z); }
