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
bool isStringValid(const std::string &name_) {
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
std::string setExpand(const std::string &name, const std::string &exp_) {
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
std::string formatStr(const char *format, ...) {
  char buffer[1024];
  va_list args;
  va_start(args, format);
  vsnprintf_s(buffer, 1024, format, args);
  va_end(args);
  return std::string(buffer);
}
