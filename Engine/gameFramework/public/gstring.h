#ifndef GSTRING_H
#define GSTRING_H
#include <functional>
#include <string>

class Gstring {
private:
  std::string str;
  size_t hash = 0;

public:
public:
  template <class T, typename = std::enable_if_t<
                         std::is_constructible_v<std::string, T>>>
  Gstring(T &&arg) : str(std::forward<T>(arg)) {
    hash = std::hash<std::string>{}(str);
  }
  Gstring(){}
  size_t get_hash() const { return hash; }

  static size_t calculateHash(const std::string &str) {
    return std::hash<std::string>{}(str);
  }

  const std::string &getStringStd() const { return str; }

public:
  void operator=(const std::string &strStd) {
    str = strStd;
    hash = std::hash<std::string>{}(strStd);
  }
  void operator=(const Gstring& gstr_){
    str=gstr_.str;
    hash=gstr_.hash;
  }
};
#endif // GSTRING_H