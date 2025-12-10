#include "base.h"
#include"filesystem"
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
