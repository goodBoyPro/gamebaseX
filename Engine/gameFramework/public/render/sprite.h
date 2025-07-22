#ifndef SPRITE_H
#define SPRITE_H
#include "base/base.h"
#include "cmath"
#include "string"
class GTexture {
  public:
    TextureBase texture;
    int column;
    int row;
    IVector2 sizeTexUnit;
    GTexture()=default;
    void init(int row_, int column_, const std::string &path) {
        texture.loadFromFile(path);
        row = row_;
        column = column_;
        sizeTexUnit = {(int)(texture.getSize().x / column_), (int)(texture.getSize().y / row_)};
    }
    GTexture(int row_, int column_, const std::string &path) {
        init(row_, column_, path);
    }
};
// 图片资源大小应为2的幂
class GSprite {
  private:
    SpriteBase sprite;
    IVector2 sizeTexUnit;
    int rows;
    int columns;
    int curId = 0;

  public:
    GSprite() =default;
    void init(const GTexture &textureArray) {
        rows = textureArray.row;
        columns = textureArray.column;
        sizeTexUnit = textureArray.sizeTexUnit;
        sprite.setTexture(textureArray.texture);
        setId(0);
    }
    GSprite(const GTexture &textureArray) {
        init(textureArray);
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