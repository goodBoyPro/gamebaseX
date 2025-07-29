#include"stdio.h"
#include"fstream"
#include "string"
#include"iostream"
int main(){
    std::ifstream ifile;
    ifile.open("fileTest.txt", std::ios::binary);
    char c[10]={0};
    std::streambuf *buf = ifile.rdbuf();
    buf->sgetn(c, 9);
    std::cout << c << std::endl;
    buf->sgetn(c, 9);
    std::cout << c << std::endl;
    return 0; 
}