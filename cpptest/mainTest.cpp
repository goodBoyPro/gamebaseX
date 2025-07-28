#include"stdio.h"
#include"fstream"
#include"string"
int main(){
    printf("cpptest");
    std::ifstream ifile;
    ifile.open("fileTest.txt",std::ios::binary);
    ifile.read(char *Str, streamsize Count)
    printf(str.c_str());
    return 0;
}