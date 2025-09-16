#ifndef BASE_H
#define BASE_H

#include <SFML/Graphics.hpp>
typedef sf::Vector3f FVector3;
typedef sf::Glsl::Vec4 FVector4 ;
typedef sf::Vector2f FVector2;
typedef sf::Vector2i IVector2;
typedef sf::RenderWindow WindowBase;
typedef sf::Event EventBase;
typedef sf::Sprite SpriteBase;
typedef sf::Texture TextureBase;
typedef sf::Color ColorBase;
typedef sf::Clock GameClock;
class GameWindow : public WindowBase {
  class GCameraObj *cameraActive = nullptr;

public:
  void setCameraActive(GCameraObj *cameraComp_) {
    cameraActive = cameraComp_;
  }
  GCameraObj *getCameraActve() { return cameraActive; }
  IVector2 wsToWin(const FVector3 &posWs_);
  FVector3 winToWs(FVector2 posWin_);
  FVector3 getMousePositionWs();
  FVector2 getMousePositionWin();
};

// 检查文件是否存在
bool isFileExist(const std::string &filePath);

// 检查文件夹是否存在
bool isFolderExist(const std::string &folderPath);

// 如果文件夹不存在则创建
void createFolderIfNotExist(const std::string &folderPath);
inline bool isStringValid(const std::string &name_) {
  if (name_.empty()) {
      return false;
  } 
  for (char c : name_) {
      // 注意：必须转换为 unsigned char 避免未定义行为
      if (!std::isspace(static_cast<unsigned char>(c))) {
          return true; // 存在有效字符
      }
  }
  return false; // 全为空白字符
}
// 检查name是否以exp_结尾，如果不是则添加exp_
inline std::string setExpand(const std::string& name, const std::string& exp_) {
  // 如果后缀为空，直接返回原字符串
  if (exp_.empty()) {
      return name;
  }
  
  // 如果原字符串长度小于后缀长度，直接添加后缀
  if (name.length() < exp_.length()) {
      return name + exp_;
  }
  
  // 计算需要比较的起始位置
  size_t startPos = name.length() - exp_.length();
  
  // 比较字符串末尾与后缀是否相同
  bool endsWithExp = true;
  for (size_t i = 0; i < exp_.length(); ++i) {
      if (name[startPos + i] != exp_[i]) {
          endsWithExp = false;
          break;
      }
  }
  
  // 如果不相同则添加后缀，否则返回原字符串
  if (!endsWithExp) {
      return name + exp_;
  } else {
      return name;
  }
}
#endif // BASE_H