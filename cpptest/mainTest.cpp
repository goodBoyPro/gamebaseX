#include"fileNode.h"
#include"stdio.h"
int main(){
    FileTree ft;
    ft.loadTree("G:/beifen");
    for(FileNode* child:ft.root.childs[0]->childs){
        printf("name:%s\npath:%s\ntype:%d\nexpand:%s\n",child->name.c_str(),child->path.c_str(),child->type,child->expand.c_str());
    }
    return 0;
}
