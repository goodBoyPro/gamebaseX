#include "base.h"
#include "framework.h"
IVector2 GameWindow::wsToWin(const FVector3 &posWs_) {
  
  return cameraActive->wsToWin(posWs_, getDefaultView().getSize().x,
                               getDefaultView().getSize().y);
}
FVector3 GameWindow::winToWs(FVector2 posWin_) {
  return cameraActive->winToWs(posWin_, *this);
}
FVector3 GameWindow::getMousePositionWs() {
  return cameraActive->getMousePositionWs(*this);
}
FVector2 GameWindow::getMousePositionWin() {
  const IVector2 &posWin = sf::Mouse::getPosition(*this);
  return {(posWin.x * this->getDefaultView().getSize().x / this->getSize().x),
          (posWin.y * this->getDefaultView().getSize().y / this->getSize().y)};
};
bool isFileExist(const std::string &filePath) {
  return std::filesystem::exists(filePath) &&
         std::filesystem::is_regular_file(filePath);
}
bool isFolderExist(const std::string &folderPath) {
  return std::filesystem::exists(folderPath) &&
         std::filesystem::is_directory(folderPath);
}
void createFolderIfNotExist(const std::string &folderPath) {
  if (!isFolderExist(folderPath)) {
    // 递归创建目录（包括任何必要的父目录）
    std::filesystem::create_directories(folderPath);
  }
}
