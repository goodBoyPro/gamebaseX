#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include "filesystem"
#include <vector>
class FileManager {
public:
  struct FileInfo {
    std::string name;
    std::string path;
  };
  std::string rootDir = "./";
  std::vector<FileInfo> filesPng;
  std::vector<FileInfo> filesGmat;
  std::vector<FileInfo> filesGmap;
  void collectFiles(const std::string &directoryPath);
  static FileManager &getFileManager() {
    static FileManager instance;
    
    return instance;
  }
  void updateFiles() { collectFiles(rootDir); }

private:
  // 移除路径开头的"./"
  std::string removeLeadingDotSlash(const std::string &path);
  FileManager(){collectFiles(rootDir);}
};
#endif // FILEMANAGER_H
