#include"fileNode.h"
#include"stdio.h"
#include "stringCode.h"
#include "nlohmann_json/json.hpp"
#include<fstream>
int main(){
  std::ifstream ifile;
  nlohmann::json obj;
  ifile>>obj;
    
    return 0;
}
