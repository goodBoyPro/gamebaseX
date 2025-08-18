#ifndef GSOURCE_H
#define GSOURCE_H
#include "sprite.h"
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <map>
#include <vector>

class GSourceIF {
public:
  std::vector<std::string> splitString(const std::string &filename) {
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
  collectFiles(const std::string &directory, const std::string &extension) {
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
              std::vector<std::string> &&strPieces =
                  splitString(entry.path().filename().string());
              strPieces.push_back(entry.path().string());
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
  virtual ~GSourceIF() {}
};
class GSource : public GSourceIF {
  enum Info {
    etype, //
    eid,
    elevel,
    erow,
    ecolumn,
    ecenterX,
    ecenterY,
    ename,
    epath,
    count
  };
  std::map<int, GTexture> allTexture;
  GTexture defaultTex;

public:
  static GSource &getSource() {
    static GSource sr;
    return sr;
  }
  GSource() {
    std::vector<std::vector<std::string>> vec = collectFiles("res", ".png");
    for (std::vector<std::string> &cvec : vec) {
      if(cvec.size()<count)continue;
      try {
        int id = std::stoi(cvec[eid]);
        int row = std::stoi(cvec[erow]);
        int column = std::stoi(cvec[ecolumn]);
        float centerX = std::stof(cvec[ecenterX]);
        float centerY = std::stof(cvec[ecenterY]);
        std::string path = cvec[epath];
        allTexture[id].init(row, column, centerX, centerY, path);
       
        
      } catch (const std::exception &e) {
        std::cerr << "invalid file! " << e.what() << std::endl;
        continue;
      }
    }
  }
  GTexture &getTexture(int id) {
    auto it=allTexture.find(id);
    if(it==allTexture.end())return defaultTex;
    return it->second;
  }
};
#endif // GSOURCE_H