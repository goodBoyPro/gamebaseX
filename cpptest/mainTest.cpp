#include"stdio.h"
#include"fstream"
#include "string"
#include"iostream"
#include"base/saveObject.h"

struct Save:public SaveObject{
    int a;
    float b;
};
int main(){
    Save s;
    s.a=15;
    s.b=1.2;
    s.saveToFile("filetest", sizeof s);
    Save s2;
    s2.loadFromFile("filetest", sizeof s2);
    printf("%d,%f",s2.a,s2.b);

    return 0; 
}