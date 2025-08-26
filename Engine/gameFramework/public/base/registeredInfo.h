#ifndef REGISTEREDINFO_H
#define REGISTEREDINFO_H
#include "framework.h"
class ClassInfo {
private:
  static ClassInfo a;

public:
  std::map<std::string, GStaticActor::Info> staticiActors;
  ClassInfo() {
    std::ifstream ifile;
    ifile.open("res/base/data/actorData.json");
    nlohmann::json jsonObj;
    ifile >> jsonObj;
    ifile.close();
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
  }

public:
  static GStaticActor::Info &getStaticActorInfo(const std::string &name) {
    auto it = a.staticiActors.find(name);
    if (it == a.staticiActors.end()) {
      printf("%s:no such staticActor\n", name.c_str());
      throw(std::runtime_error(""));
    }
    return it->second;
  }
};
inline ClassInfo ClassInfo::a;
#endif // REGISTEREDINFO_H