#include"fileNode.h"
#include"stdio.h"
#include"stringCode.h"
int main(){
    FileTree ft;
    ft.loadTree("G:/beifen");
    for(FileNode* child:ft.root.childs[0]->childs){
        printf("name:%s\npath:%s\ntype:%d\nexpand:%s\n",utf8_to_gbk(child->name).c_str(),utf8_to_gbk(child->path).c_str(),child->type,child->expand.c_str());
        printf("-------------------------------------------\n");
    }
    
    printf("%s\n",utf8_to_gbk(ft.root.childs[0]->childs[0]->name).c_str());
    printf("%s\n",wstringToGbk(L"小明").c_str());
    
    return 0;
}
