#ifndef GSOURCE_H
#define GSOURCE_H
#include "gstring.h"
#include "sprite.h"
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <map>
#include <regex>
#include <thread>
#include <vector>

class GSourceIF {
public:
  std::vector<std::string> splitString(const std::string &filename);
  std::vector<std::vector<std::string>>
  collectFiles(const std::string &directory, const std::string &extension);
  virtual ~GSourceIF() {}
  bool isNumber(const std::string &str) {
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
};
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
template <class T> class GReourceTree : public GSourceIF {
protected:
  std::map<size_t, T> data;
  T defaultObj;

public:
  T &getObject(size_t id) {
    auto it = data.find(id);
    if (it == data.end())
      return defaultObj;

    return it->second;
  }
  T &getObject(const Gstring &str) {
    auto it = data.find(str.get_hash());
    if (it == data.end()) {
      T &s = loadFromPath(str);
      return s;
    }
    return it->second;
  }
  void removeObject(const Gstring &str) {
    auto it = data.find(str.get_hash());
    if (it == data.end()) {
      return;
    }
    data.erase(it);
  }
  virtual T &loadFromPath(const Gstring &path_) {
    printf("error");
    return defaultObj;
  }
  T &emplace(const std::string &path_) {
    size_t id = Gstring::calculateHash(path_);
    const auto &pair =
        data.emplace(std::piecewise_construct, std::forward_as_tuple(id),
                     std::forward_as_tuple());
    if (!pair.second) {
      throw std::overflow_error("Resource error: hash confilcted--" + path_);
    }
    pair.first->second.idAndPath = path_;
    return pair.first->second;
  }
};
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class GSource : public GReourceTree<GTexture> {
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
  static GSource &getSource() {
    static GSource sr;
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
  GSource() {

    // loadResource();
  }
  ~GSource() {}
};
#endif // GSOURCE_H