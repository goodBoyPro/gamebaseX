#ifndef REGISTEREDINFO_H
#define REGISTEREDINFO_H
#include "framework.h"
class ClassInfo {
private:
  static ClassInfo a;

public:
  std::map<std::string, GStaticActor::Info> staticiActors;
  static std::map<std::string, GStaticActor::Info>&getStaticActors(){return a.staticiActors;}
  ClassInfo();

public:
  static GStaticActor::Info &getStaticActorInfo(const std::string &name) {
    auto it = a.staticiActors.find(name);
    if (it == a.staticiActors.end()) {
      printf("%s:no such staticActor\n", name.c_str());
      throw(std::runtime_error(""));
    }
    return it->second;
  }
  static void saveAllInfo();
  static void registerStaticActors(const std::string &name_,
                                   const std::string &texPath_, int texIndex_) {
    if (!isStringValid(name_)) {
      printf("invalid name\n");
      return;}
    auto it = a.staticiActors.find(name_);
    if (it != a.staticiActors.end()) {
      printf("%s cannot registered because exists",name_.c_str());
      return ;
    }
    size_t id = Gstring::calculateHash(name_);
    size_t texId = Gstring::calculateHash(texPath_);
    int texIndex = texIndex_;
    auto it2 = a.staticiActors.emplace(std::piecewise_construct,
                                     std::forward_as_tuple(name_),
                                     std::forward_as_tuple());
    it2.first->second.name=name_;
    it2.first->second.id=id;
    it2.first->second.texId=texId;
    it2.first->second.texIndex=texIndex;
    it2.first->second.texPath=texPath_;
    
  }
  static void unRegisterStaticActor(const std::string &name_) {
    auto it = a.staticiActors.find(name_);
    if (it != a.staticiActors.end()) {
      a.staticiActors.erase(it);
    }
  }
};

#endif // REGISTEREDINFO_H