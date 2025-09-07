#include "registeredInfo.h"
ClassInfo ClassInfo::a;
ClassInfo::ClassInfo() {
  std::ifstream ifile;
  ifile.open("res/base/data/actorData.json");
  nlohmann::json jsonObj;
  ifile >> jsonObj;
  ifile.close();
  // 读取所有staticActor
  for (auto &info : jsonObj["staticActors"]) {
    const std::string &name = info["name"];
    size_t id = info["id"];
    size_t texId = info["texId"];
    const std::string &texPath = info["texPath"];
    int texIndex = info["texIndex"];
    auto it = staticiActors.emplace(std::piecewise_construct,
                                    std::forward_as_tuple(name),
                                    std::forward_as_tuple());
    if (!it.second) {
      throw std::runtime_error(name + ":staticActorNameRedefined\n");
    }
    it.first->second.name = name;
    it.first->second.id = id;
    it.first->second.texId = texId;
    it.first->second.texIndex = texIndex;
    it.first->second.texPath = texPath;
  }
  //
}
void ClassInfo::saveAllInfo() {
  std::ofstream ofile;
  ofile.open("res/base/data/actorData.json");
  // 保存所有staticActor
  nlohmann::json jobj;
  nlohmann::json &jStaticActors = jobj["staticActors"];
  for (auto &info : a.staticiActors) {
    nlohmann::json j;
    j["name"] = info.second.name;
    j["id"] = info.second.id;
    j["texId"] = info.second.texId;
    j["texIndex"] = info.second.texIndex;
    j["texPath"] = info.second.texPath;
    jStaticActors.push_back(j);
  }
  ////////
  ofile << jobj.dump(4);
  ofile.close();
}
