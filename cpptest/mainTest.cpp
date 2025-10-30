#include"fileNode.h"
#include"stdio.h"
std::string utf8_to_gbk(const std::string& utf8_str) {
    // 步骤1：先将UTF-8转换为UTF-16（Windows宽字符，作为中间编码）
    // 先获取转换所需的宽字符缓冲区长度（包含终止符）
    int wide_char_len = MultiByteToWideChar(
        CP_UTF8,         // 源编码类型：UTF-8
        0,               // 转换标志：0表示默认行为
        utf8_str.c_str(),// 输入的UTF-8字符串
        -1,              // 输入长度：-1表示自动计算（包含\0终止符）
        nullptr,         // 输出缓冲区：先传nullptr获取所需长度
        0                // 输出缓冲区大小：0表示仅计算长度
    );
    if (wide_char_len == 0) {
        // 转换失败（可能是UTF-8格式无效或系统不支持）
        return "";
    }

    // 分配宽字符缓冲区并执行转换
    wchar_t* wide_buf = new (std::nothrow) wchar_t[wide_char_len];
    if (!wide_buf) {
        // 内存分配失败
        return "";
    }
    // 实际执行UTF-8到UTF-16的转换
    if (MultiByteToWideChar(
        CP_UTF8, 
        0, 
        utf8_str.c_str(), 
        -1, 
        wide_buf, 
        wide_char_len
    ) == 0) {
        delete[] wide_buf;
        return "";
    }

    // 步骤2：将UTF-16（宽字符）转换为GBK编码（代码页936）
    // 先获取GBK缓冲区所需长度（包含终止符）
    int gbk_len = WideCharToMultiByte(
        936,             // 目标编码：GBK对应的代码页（936）
        0,               // 转换标志：0表示默认行为
        wide_buf,        // 输入的UTF-16宽字符
        -1,              // 输入长度：-1表示自动计算（包含\0终止符）
        nullptr,         // 输出缓冲区：先传nullptr获取所需长度
        0,               // 输出缓冲区大小：0表示仅计算长度
        nullptr,         nullptr  // 不使用默认替换字符
    );
    if (gbk_len == 0) {
        delete[] wide_buf;
        return "";
    }

    // 分配GBK缓冲区并执行转换
    char* gbk_buf = new (std::nothrow) char[gbk_len];
    if (!gbk_buf) {
        delete[] wide_buf;
        return "";
    }
    // 实际执行UTF-16到GBK的转换
    if (WideCharToMultiByte(
        936, 
        0, 
        wide_buf, 
        -1, 
        gbk_buf, 
        gbk_len, 
        nullptr, 
        nullptr
    ) == 0) {
        delete[] wide_buf;
        delete[] gbk_buf;
        return "";
    }

    // 构造结果字符串（自动忽略末尾的\0终止符）
    std::string gbk_str(gbk_buf);

    // 清理动态分配的内存
    delete[] wide_buf;
    delete[] gbk_buf;

    return gbk_str;
}
int main(){
    FileTree ft;
    ft.loadTree("G:/beifen");
    // for(FileNode* child:ft.root.childs[0]->childs){
    //     printf("name:%s\npath:%s\ntype:%d\nexpand:%s\n",child->name.c_str(),child->path.c_str(),child->type,child->expand.c_str());
    // }
    printf("%s\n",ft.root.childs[0]->childs[0]->name.c_str());
    printf("%s",utf8_to_gbk(ft.root.childs[0]->childs[0]->name).c_str());
    return 0;
}
