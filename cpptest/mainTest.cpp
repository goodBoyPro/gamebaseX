#include"stdio.h"
#include"fstream"
#include "string"
#include"iostream"
struct Wtest{int a=10;float b=0.5;};
int main(){
    std::ifstream ifile;
    ifile.open("fileTest.txt", std::ios::binary);
    char c[10]={0};
    std::streambuf *buf = ifile.rdbuf();
    
    std::ofstream ofile;
    ofile.open("writeTest.txt",std::ios::binary);
    Wtest w;
    ofile.write((char*)(&w), sizeof w);
    return 0; 
}