#ifndef FILE_NODE_H
#define FILE_NODE_H
#include"vector"
#include<string>
#include<filesystem>
#include"iostream"
#include"windows.h"
class FileNode{  
    
    public:
    enum NodeType { eFile, eDir }type=eFile;
    FileNode*parent=nullptr;
    std::vector<FileNode*>childs;
    std::string name;
    std::string path;
    std::string expand;
    void clear(){
        for(FileNode*child:childs){
            delete child;
        }
    }
    ~FileNode(){
        clear();
    }

};
class FileTree{
    void addChildren(FileNode* parentNode, const std::filesystem::path& currentDir);
    public:
    FileNode root;
    void loadTree(const std::string&rootPath);
   

    
};
//////////////////////////////////////////
inline void FileTree::addChildren(FileNode* parentNode, const std::filesystem::path& currentDir) {
        namespace fs = std::filesystem;
        try {
            // 遍历当前目录下的所有条目
            for (const auto& entry : fs::directory_iterator(currentDir)) {
                fs::path entryPath = entry.path();
                FileNode* child = new FileNode();

                // 设置子节点基本属性
                child->parent = parentNode;
                child->name =(entryPath.filename().string());
                child->path =(entryPath.string());

                // 判断条目类型并处理
                if (entry.is_directory()) {
                    child->type = FileNode::eDir;
                    child->expand = "";
                    // 递归处理子目录
                    addChildren(child, entryPath);
                } else if (entry.is_regular_file()) {
                    child->type = FileNode::eFile;
                    child->expand = entryPath.extension().string(); // 扩展名（如".txt"）
                } else {
                    // 忽略非普通文件/目录（如符号链接、管道等）
                    delete child;
                    continue;
                }

                // 添加到父节点的子列表
                parentNode->childs.push_back(child);
            }
        } catch (const fs::filesystem_error& e) {
            // 处理目录访问错误（如权限不足）
            std::cerr << "Error accessing directory " << currentDir << ": " << e.what() << std::endl;
        }
    }
inline void FileTree::loadTree(const std::string& rootPath) {
        root.clear(); // 清空现有树结构
        namespace fs = std::filesystem;
        fs::path rootFsPath(rootPath);

        // 校验根路径合法性
        if (!fs::exists(rootFsPath)) {
            std::cerr << "Error: Path does not exist - " << rootPath << std::endl;
            return;
        }
        if (!fs::is_directory(rootFsPath)) {
            std::cerr << "Error: Path is not a directory - " << rootPath << std::endl;
            return;
        }

        // 初始化根节点
        root.parent = nullptr;
        root.path = (rootFsPath.string());
        root.name = (rootFsPath.filename().string());
        root.type = FileNode::eDir;
        root.expand = "";
        root.childs.clear(); // 清空原有子节点（避免重复加载）

        // 调用辅助函数递归构建子节点
        addChildren(&root, rootFsPath);
    }
#endif //FILE_NODE_H