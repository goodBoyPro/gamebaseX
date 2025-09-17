#ifndef EDITORCONFIG_H
#define EDITORCONFIG_H
#include "fstream"
#include "nlohmann_json/json.hpp"
class EditorConfig {
    EditorConfig() {
    loadData();
  }
public:
  static EditorConfig &getEditorConfig() {
    static EditorConfig instance;
    return instance;
  }
  void loadData() {
    std::ifstream ifile("editorData/editorConfig.json");
    nlohmann::json jObj;
    ifile >> jObj;
    ifile.close();
  }
  ~EditorConfig() {}
  // config
};
inline EditorConfig&getEditorConfig() {
  return EditorConfig::getEditorConfig();
}
#endif // EDITORCONFIG_H