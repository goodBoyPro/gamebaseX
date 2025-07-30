#ifndef SAVEOBJECT_H
#define SAVEOBJECT_H
#include "string"
#include "fstream"
struct SaveObject {
    bool saveToFile(const std::string &path, int size) {
        std::ofstream ofile;
        ofile.open(path,std::ios::binary);
        if(!ofile.is_open()){return false;}
        ofile.write((char *)this, size);
        ofile.close();
        return true;
    };
    bool loadFromFile(const std::string &path, int size) {
        std::ifstream ifile;
        ifile.open(path,std::ios::binary);
        if (!ifile.is_open())
            return false;
        ifile.read((char *)this, size);
        return true;
    };
};
#endif // SAVEOBJECT_H