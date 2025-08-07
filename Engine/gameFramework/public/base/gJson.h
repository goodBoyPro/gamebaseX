#ifndef GJSON_H
#define GJSON_H
#include "nlohmann_json/json.hpp"
#include <fstream>
//obj["str"];obj.push_back();obj=nlohmann::json({1,2,3});
class Gjson {
  nlohmann::json js;
  std::string path;
  bool loadFromFile(const std::string &path_) {
    std::ifstream inFile;
    inFile.open(path_);
    if (!inFile.is_open()) {
      printf("file %s open failed\n", path_.c_str());
      return false;
    }
    inFile >> js;
    inFile.close();
    return true;
  }
  void saveToFile(const std::string &path) {
    std::ofstream outFile;
    outFile.open(path);
    const std::string &jsonText = js.dump(4);
    outFile.write(jsonText.data(), jsonText.size());
    outFile.close();
  }

public:
  nlohmann::json &operator[](const std::string &str) { return js[str]; }

  Gjson(const std::string &path_) {
    path = path_;
    bool isFileExsit = loadFromFile(path_);
    if (!isFileExsit) {
      std::ofstream outFile;
      outFile.open(path_);
      const std::string &str = js.dump();
      outFile.write(str.data(), str.size());
      outFile.close();
      printf("file created:%s", path_.c_str());
    }
  }
  void save() { saveToFile(path); }
};
#endif // GJSON_H