#ifndef SPRITE_H
#define SPRITE_H
#include "base/base.h"
#include "cmath"
// 图片资源大小应为2的幂
class GSprite {
  private:
    SpriteBase sprite;
    IVector2 sizeTexUnit;
    int rows;
    int columns;
    int curId = 0;

  public:
    GSprite(GTexture &textureArray, int rows_, int columns_) {
        rows = rows_;
        columns = columns_;
        sizeTexUnit = {(int)(textureArray.getSize().x / columns), (int)(textureArray.getSize().y / rows)};
        sprite.setTexture(textureArray);
        setId(0);
    }
    void setId(int index) {
        if (index >= rows * columns)
            index = rows * columns - 1;
        sprite.setTextureRect({index % columns * sizeTexUnit.x, index / columns * sizeTexUnit.y, sizeTexUnit.x, sizeTexUnit.y});
        curId = index;
    }
    void setIdGood(int index) {
        curId = index;
        const int column_mask = columns - 1;
        int x = (index & column_mask) * sizeTexUnit.x;
        int y = (index >> (int)log2(columns)) * sizeTexUnit.y;
        sprite.setTextureRect({x, y, sizeTexUnit.x, sizeTexUnit.y});
    }
    void setSizeWin(float x, float y) { sprite.setScale(x / sizeTexUnit.x, y / sizeTexUnit.y); }
    void setPositionWin(float x, float y) { sprite.setPosition(x, y); }
    void setCenter(float x, float y) { sprite.setOrigin(x * sizeTexUnit.x, y * sizeTexUnit.y); }
    void drawWin(WindowBase &window_) { window_.draw(sprite); }
};
class GAnimation : public GSprite {};
#endif // SPRITE_H