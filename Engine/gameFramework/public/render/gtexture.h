#ifndef GTEXTURE_H
#define GTEXTURE_H
#include "gsource.h"
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
class GTextureTree : public GReourceTree<GTexture> {
  enum Info {
    epath,
    etype, //
    eid,
    elevel,
    erow,
    ecolumn,
    ecenterX,
    ecenterY,
    ename,

    count
  };

public:
  static GTextureTree &getSource() {
    static GTextureTree sr;
    return sr;
  }
  // 已弃用
  void loadResource() {
    std::vector<std::vector<std::string>> vec = collectFiles("res", ".png");
    for (std::vector<std::string> &cvec : vec) {
      const std::string &path = cvec[0];
      GTexture &gtex = emplace(path);
      if (cvec.size() != count) {
        gtex.init(1, 1, 0, 0, path);
        continue;
      }
      try {
        int row = std::stoi(cvec[erow]);
        int column = std::stoi(cvec[ecolumn]);
        float centerX = std::stof(cvec[ecenterX]);
        float centerY = std::stof(cvec[ecenterY]);

        gtex.init(row, column, centerX, centerY, path);

      } catch (const std::exception &e) {
        std::cerr << "invalid file! " << e.what() << std::endl;
        gtex.init(1, 1, 0, 0, path);
        continue;
      }
    }
  }
  GTexture &loadFromPath(const Gstring &path_) {

    const std::vector<std::string> &strs = splitString(
        std::filesystem::path(path_.getStringStd()).filename().string());
    const std::string &path = path_.getStringStd();
    // 检查格式
    bool isRightFormat = true;
    if (strs.size() < 4) {
      isRightFormat = false;
    } else {
      int num=strs.size()-1;
      for (int i=0;i<4;i++){       
        if(!isNumber(strs[num-i])){
          isRightFormat = false;
          break;
        }
      }
    }
    GTexture &gtex = emplace(path);

    if (isRightFormat) {
     int num=strs.size()-1;
      int row = std::stoi(strs[num - 3]);
      int column = std::stoi(strs[num - 2]);
      float centerX = std::stof(strs[num - 1]);
      float centerY = std::stof(strs[num]);

      gtex.init(row, column, centerX, centerY, path);
      return gtex;
    }
    gtex.init(1, 1, 0, 0, path);
    return gtex;
    // try {
    //   int row = std::stoi(strs[erow - 1]);
    //   int column = std::stoi(strs[ecolumn - 1]);
    //   float centerX = std::stof(strs[ecenterX - 1]);
    //   float centerY = std::stof(strs[ecenterY - 1]);

    //   gtex.init(row, column, centerX, centerY, path);
    //   return gtex;

    // } catch (const std::exception &e) {
    //   std::cerr << "invalid file! " << e.what() << std::endl;
    //   gtex.init(1, 1, 0, 0, path);
    //   return gtex;
    // }
  };
  GTextureTree() {

    // loadResource();
  }
  ~GTextureTree() {}
};
#endif// GTEXTURE_H