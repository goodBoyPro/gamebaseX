#include "drop.hpp"
#include <windows.h>
#include <vector>
#include <locale>
#include <codecvt>

// 全局变量用于存储回调函数
static FileDropCallback s_fileDropCallback = nullptr;

// 窗口过程回调函数，用于处理WM_DROPFILES消息
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_DROPFILES && s_fileDropCallback) {
        HDROP hDrop = (HDROP)wParam;
        
        // 获取拖放的文件数量
        UINT fileCount = DragQueryFile(hDrop, 0xFFFFFFFF, nullptr, 0);
        
        // 处理每个文件
        for (UINT i = 0; i < fileCount; ++i) {
            // 获取文件名长度
            UINT fileNameLength = DragQueryFile(hDrop, i, nullptr, 0) + 1;
            
            // 分配缓冲区并获取文件名
            std::vector<wchar_t> fileNameBuffer(fileNameLength);
            DragQueryFileW(hDrop, i, fileNameBuffer.data(), fileNameLength);
            
            // 转换为UTF-8字符串
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            std::string filePath = converter.to_bytes(fileNameBuffer.data());
            
            // 调用回调函数
            s_fileDropCallback(filePath);
        }
        
        // 释放拖放数据
        DragFinish(hDrop);
        return 0;
    }
    
    // 调用默认窗口过程
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// 启用文件拖放功能
void enableFileDrop(sf::Window& window) {
    // 获取窗口句柄
    HWND hwnd = (HWND)window.getSystemHandle();
    
    // 设置新的窗口过程
    SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)WndProc);
    
    // 允许窗口接受文件拖放
    DragAcceptFiles(hwnd, TRUE);
}

// 处理文件拖放消息
void handleFileDropMessages(FileDropCallback callback) {
    s_fileDropCallback = callback;
}
    