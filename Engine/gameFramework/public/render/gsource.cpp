#include "gsource.h"
std::vector<std::string> GSourceTreeIF::splitString(const std::string &filename) {
  std::vector<std::string> tokens;

  // 分离文件名与扩展名
  size_t dot_pos = filename.rfind('.');
  std::string base_name =
      (dot_pos != std::string::npos) ? filename.substr(0, dot_pos) : filename;

  // 以下划线分割并过滤空字段
  std::istringstream iss(base_name);
  std::string token;
  while (getline(iss, token, '_')) {
    if (!token.empty()) { // 跳过连续下划线产生的空字段
      tokens.push_back(token);
    }
  }

  return tokens;
}
std::vector<std::vector<std::string>>
GSourceTreeIF::collectFiles(const std::string &directory,
                        const std::string &extension) {
  std::vector<std::vector<std::string>> fileStringTree;
  try {
    // 递归遍历目录
    for (const auto &entry :
         std::filesystem::recursive_directory_iterator(directory)) {
      try {
        if (entry.is_regular_file()) {
          // 获取文件扩展名并转换为小写
          std::string ext = entry.path().extension().string();
          std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

          if (ext == extension) {
            std::vector<std::string> strPieces;
            strPieces.push_back(entry.path().generic_string());
            auto strPieces0=splitString(entry.path().filename().string());
            strPieces.insert(strPieces.end(),strPieces0.begin(),strPieces0.end());   
            
            fileStringTree.push_back(strPieces);
          }
        }
      } catch (const std::filesystem::filesystem_error &e) {
        std::cerr << "文件访问错误: " << e.what() << std::endl;
      }
    }
  } catch (const std::filesystem::filesystem_error &e) {
    std::cerr << "目录遍历错误: " << e.what() << std::endl;
  }

  return fileStringTree;
}
bool GSourceTreeIF::isNumber(const std::string &str) {
  // 空字符串不是数值
  if (str.empty()) {
    return false;
  }

  size_t i = 0;
  // 处理可选的正负号
  if (str[i] == '+' || str[i] == '-') {
    i++;
    // 如果只有符号，不是数值
    if (i == str.size()) {
      return false;
    }
  }

  bool hasDigit = false; // 标记是否出现过数字
  bool hasDot = false;   // 标记是否出现过小数点

  for (; i < str.size(); ++i) {
    if (isdigit(str[i])) {
      // 遇到数字，标记已出现数字
      hasDigit = true;
    } else if (str[i] == '.') {
      // 遇到小数点：不能出现过多个小数点，且至少需要有数字（前或后）
      if (hasDot) {
        return false; // 多个小数点无效
      }
      hasDot = true;
    } else {
      // 其他字符均为无效
      return false;
    }
  }

  // 必须至少出现过一个数字才是有效的数值
  return hasDigit;
}
