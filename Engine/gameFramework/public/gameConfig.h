#ifndef GAMECONFIG_H
#define GAMECONFIG_H
#include "fstream"
#include "nlohmann_json/json.hpp"

class GameConfig {
  GameConfig() { loadData(); };

public:
  static GameConfig &getGameConfig() {
    static GameConfig ret;
    return ret;
  }
  void loadData() {
    std::ifstream iflile("system/config.json");
    if (!iflile.is_open()) {
      printf("no config file, use default config\n");
      return;
    }
    nlohmann::json jObj;
    iflile >> jObj;
    iflile.close();
    ///////////////////////////读取配置
    try {
      windowWidth = jObj["windowWidth"];
      windowHeight = jObj["windowHeight"];
      frameLimit = jObj["frameLimit"];
    } catch (const std::exception &e) {
      printf("config file format error, use default config\n");
    }
  }
  // config
public:
  int windowWidth = 1280;
  int windowHeight = 720;
  int frameLimit = 60;
  std::string windowIcon = "system/texture/a.png";
};
inline GameConfig &getGameConfig() { return GameConfig::getGameConfig(); }
#endif // GAMECONFIG_H