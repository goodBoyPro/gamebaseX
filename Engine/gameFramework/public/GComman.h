#ifndef GCOMMAN_H
#define GCOMMAN_H
#include "base/base.h"
#include "math.h"

////////////////////////////////////////////////////////////////////////////////
/// 文字相关////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
sf::Font &getFont(int id);
// 缺省值应该写在声明中才会重载
// 输出文字，参数列表：文字内容，位置x,位置y,尺寸，颜色，字体
void printText(WindowBase &window_, const std::wstring &str = L"未输入文字", int x = 0, int y = 0, int size = 30, sf::Color color = ColorBase(255, 255, 255), int fontId = 0);
void printTextLimit(WindowBase &window_, const std::wstring &str = L"未输入文字", float x = 0, float y = 0, int size = 30, const FVector2 &sprSize = {1, 1},
                    const ColorBase &color = ColorBase(255, 255, 255), int fontId = 0);
void printNum(WindowBase &window_, float __float, int x = 0, int y = 0, int size = 30, sf::Color color = ColorBase(255, 255, 255), int fontId = 0);
void printNum(WindowBase &window_, int __int, int x = 0, int y = 0, int size = 30, sf::Color color = ColorBase(255, 255, 255), int fontId = 0);

////////////////////////////////////////////////////////////////////////////////
/// 向量相关////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
float getVectorLen(const FVector3 &vec);
FVector3 vectorNormalize(const FVector3 &vec);

#endif // GCOMMAN_H