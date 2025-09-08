#include"fileManager.h"
int main(){
  auto &fm=FileManager::getFileManager();
  for(auto &f:fm.filesPng){
    printf("png:%s\n",f.path.c_str());
  }
  return 0;
}