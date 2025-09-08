#include "fileManager.h"
std::string FileManager::removeLeadingDotSlash(const std::string &path) {
  // 检查路径是否以"./"开头
  if (path.size() >= 2 && path[0] == '.' && path[1] == '/') {
    // 从第三个字符开始截取字符串（移除前两个字符）
    return path.substr(2);
  }
  // 不满足条件则返回原路径
  return path;
}
void FileManager::collectFiles(const std::string &directoryPath) {
  filesPng.clear();
  filesGmat.clear();
  filesGmap.clear();
  for (const auto &entry :
       std::filesystem::recursive_directory_iterator(directoryPath)) {
    if (entry.is_regular_file()) {
      std::string filePath =
          removeLeadingDotSlash(entry.path().generic_string());
      std::string fileName = entry.path().filename().string();
      std::string extension = entry.path().extension().string();
      if (extension == ".png") {
        filesPng.push_back({fileName, filePath});
      } else if (extension == ".gmat") {
        filesGmat.push_back({fileName, filePath});
      } else if (extension == ".gmap") {
        filesGmap.push_back({fileName, filePath});
      }
    }
  }
}
