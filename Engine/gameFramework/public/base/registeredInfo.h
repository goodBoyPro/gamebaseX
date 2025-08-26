#ifndef REGISTEREDINFO_H
#define REGISTEREDINFO_H
#include "framework.h"
class ClassInfo {
private:
//   inline static ClassInfo a;

public:
  inline static std::map<std::string, GStaticActor::Info> staticiActors;
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
      auto it = staticiActors.emplace(name,GStaticActor::Info());
      if (!it.second) {
        throw std::runtime_error(name + ":staticActorNameRedefined\n");
      }
      it.first->second.name = name;
      it.first->second.id = id;
      it.first->second.texId = texId;
      it.first->second.texIndex = texIndex;
      it.first->second.texPath = texPath;
    }
    printf("%s", staticiActors["adfsa"].texPath.c_str());
  }
};
#endif // REGISTEREDINFO_H