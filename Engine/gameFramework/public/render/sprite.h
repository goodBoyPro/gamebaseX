#ifndef SPRITE_H
#define SPRITE_H
#include "base/base.h"
#include "cmath"
#include "gstring.h"
#include "string"

class GSourceObj {
public:
  Gstring idAndPath;

  virtual ~GSourceObj() {}
};
class GTexture : public GSourceObj {
public:
  TextureBase texture;
  int column = 1;
  int row = 1;
  float centerX = 0;
  float centerY = 0;
  // 纹理小格单元尺寸
  IVector2 sizeTexUnit;
  GTexture() {};
  void init(int row_, int column_, float centerX_, float centerY_,
            const std::string &path) {
    if (!texture.loadFromFile(path)) {
      texture.loadFromFile("system/texture/invalidTex.png");     
      sizeTexUnit = {(int)(texture.getSize().x / column_),
                     (int)(texture.getSize().y / row_)};
      return;
    };
    texture.generateMipmap();
    texture.setSmooth(true);
    row = row_;
    column = column_;
    centerX = centerX_;
    centerY = centerY_;
    sizeTexUnit = {(int)(texture.getSize().x / column_),
                   (int)(texture.getSize().y / row_)};
  }
  GTexture(int row_, int column_, float centerX_, float centerY_,
           const std::string &path) {
    init(row_, column_, centerX_, centerY_, path);
  }
};
// 图片资源大小应为2的幂
class GSprite {
protected:
  SpriteBase sprite;
  // 序列图每一个单元格的尺寸
  IVector2 sizeTexUnit = {1, 1};
  int rows = 1;
  int columns = 1;
  int curId = 0;
  GTexture *texturePtr = nullptr;

public:
  // 用户不要引用
  float sortFlag = 0;
  FVector3 posWs = {0, 0, 0};

public:
  GTexture *getTexturePtr() { return texturePtr; }
  bool isContainPos(float x, float y) {
    return sprite.getGlobalBounds().contains(x, y);
  }
  SpriteBase &getSpriteBase() { return sprite; }
  GSprite() = default;
  void setSortFlag(float value_) { sortFlag = value_; }
  void init(GTexture &textureArray) {
    texturePtr = &textureArray;
    rows = textureArray.row;
    columns = textureArray.column;
    sizeTexUnit = textureArray.sizeTexUnit;
    sprite.setTexture(textureArray.texture);
    setCenter(textureArray.centerX, textureArray.centerY);
    setId(0);
  }
  GSprite(GTexture &textureArray) { init(textureArray); }
  int getCurId() { return curId; }
  void setId(int index) {
    if (index >= rows * columns)
      index = rows * columns - 1;
    sprite.setTextureRect({index % columns * sizeTexUnit.x,
                           index / columns * sizeTexUnit.y, sizeTexUnit.x,
                           sizeTexUnit.y});
    curId = index;
  }
  void setIdGood(int index) {
    curId = index;
    const int column_mask = columns - 1;
    int x = (index & column_mask) * sizeTexUnit.x;
    int y = (index >> (int)log2(columns)) * sizeTexUnit.y;
    sprite.setTextureRect({x, y, sizeTexUnit.x, sizeTexUnit.y});
  }
  void setSizeWin(float x, float y) {
    sprite.setScale(x / sizeTexUnit.x, y / sizeTexUnit.y);
  }
  void setPositionWin(float x, float y) { sprite.setPosition(x, y); }
  void setRotation(float angle_) { sprite.setRotation(angle_); }
  void setCenter(float x, float y) {
    sprite.setOrigin(x * sizeTexUnit.x, y * sizeTexUnit.y);
  }
  virtual void drawWin(GameWindow &window_) { window_.draw(sprite); }
};
class GAnimation : public GSprite {
private:
  int idBegin = 0;
  int idEnd = 0;
  bool bPlaying = false;

  float updateDelayS = 0.05;
  float updateFlag = 0;

public:
  const int getIdBegin() { return idBegin; }
  const int getIdEnd() { return idEnd; }
  int getFramePerS() { return 1.f / updateDelayS; }
  GAnimation() {}
  void init(GTexture &textureArray, int beg_, int end_) {
    GSprite::init(textureArray);
    setAnimPiece(beg_, end_);
  }
  void setFramePerS(int f_) {
    if (f_ < 1)
      return;
    updateDelayS = 1.f / f_;
  }
  void setAnimPiece(int idBeg_, int idEnd_) {
    idBegin = idBeg_;
    idEnd = idEnd_;
    setId(idBegin);
  }
  void play() { bPlaying = true; }
  void playFrom(int id_) {
    bPlaying = true;
    int i = idBegin;
    if (id_ < idBegin || id_ > idEnd)
      return;
    i = id_;
    setId(i);
  }
  void stop() { bPlaying = false; }
  void loop(float deltaTime) {
    if (!bPlaying)
      return;
    if (updateFlag > updateDelayS) {
      if (getCurId() < idEnd) {
        setId(getCurId() + 1);
      } else {
        setId(idBegin);
      }
      updateFlag = 0;
    }
    updateFlag += deltaTime;
  }

  void loop2(float deltaTime) {
    if (!bPlaying)
      return;
    if (getCurId() < idEnd) {
      updateFlag = updateFlag + updateDelayS;
      setId((int)(updateFlag));
    } else {
      setId(idBegin);
      updateFlag = idBegin;
    }
  }
};
#endif // SPRITE_H