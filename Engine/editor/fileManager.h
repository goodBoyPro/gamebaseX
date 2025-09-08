#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include "filesystem"
#include<vector>
class FileManager {
public:

  struct FileInfo {
    std::string name;    
    std::string path;
  };
  std::string rootDir="./";
  std::vector<FileInfo>filesPng;
  std::vector<FileInfo>filesGmat;
  std::vector<FileInfo>filesGmap;
  void collectFiles(const std::string &directoryPath) {
    filesPng.clear();
    filesGmat.clear();
    filesGmap.clear();
    for (const auto &entry : std::filesystem::recursive_directory_iterator(directoryPath)) {
      if (entry.is_regular_file()) {
        std::string filePath = entry.path().generic_string();
        std::string fileName = entry.path().filename().string();
        std::string extension = entry.path().extension().string();
        if (extension == ".png") {
          filesPng.push_back({fileName, filePath});
        } else if (extension == ".gmat") {
          filesGmat.push_back({fileName, filePath});
        } else if (extension == ".gmap") {
          filesGmap.push_back({fileName, filePath});
        }
      }
    }
  }
  static FileManager &getFileManager() {
    static FileManager instance;
    instance.collectFiles(instance.rootDir);
    return instance;
  }
  void updateFiles() { collectFiles(rootDir); }
  
};
#endif // FILEMANAGER_H
