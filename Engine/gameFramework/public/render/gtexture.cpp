#include "gtexture.h"
SourceRefer<GTexture>GTextureTree::loadFromPath(const Gstring &path_) {

  const std::vector<std::string> &strs = splitString(
      std::filesystem::path(path_.getStringStd()).filename().string());
  const std::string &path = path_.getStringStd();
  // 检查格式
  bool isRightFormat = true;
  if (strs.size() < 4) {
    isRightFormat = false;
  } else {
    int num = strs.size() - 1;
    for (int i = 0; i < 4; i++) {
      if (!isNumber(strs[num - i])) {
        isRightFormat = false;
        break;
      }
    }
  }
  GTexture *ptr = new GTexture;
  SourceRefer<GTexture> gtex = emplace(path,ptr);
  
  // gtex=ptr->makeRefer<GTexture>();

  if (isRightFormat) {
    int num = strs.size() - 1;
    int row = std::stoi(strs[num - 3]);
    int column = std::stoi(strs[num - 2]);
    float centerX = std::stof(strs[num - 1]);
    float centerY = std::stof(strs[num]);

    gtex->init(row, column, centerX, centerY, path);
    return gtex;
  }
  gtex->init(1, 1, 0, 0, path);
  return gtex;
};