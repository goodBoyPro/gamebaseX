#include "stdio.h"
#include <windows.h>

struct STRC {
  int a = 1;
  int b = 10;
  bool isChild = true;
};
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                            LPARAM lParam) {
  STRC *data = (STRC *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
  switch (uMsg) {
  case WM_DESTROY:
    if (data&&data->isChild) {
         printf("%d", data->b);
       
    } else
      PostQuitMessage(0);
   
    return 0;
  }
  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
WNDCLASSEX wc = {0};

int main(int c, char **v) {
  STRC strc;
  wc.cbSize = sizeof(WNDCLASSEX); // 必须设置结构体大小
  wc.lpszClassName = L"TestClass";
  wc.lpfnWndProc = WindowProc;
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.hInstance = GetModuleHandleW(nullptr);
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW-4); // 设置背景画刷
  wc.hCursor = LoadCursor(nullptr, IDC_ARROW);   // 设置光标
  RegisterClassEx(&wc);

  HWND hw1 =
      CreateWindowEx(0, L"TestClass", L"Test1", WS_OVERLAPPEDWINDOW,
                     CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, // 正确设置尺寸
                     nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
  HWND hw2 = CreateWindowEx(0, L"TestClass", L"Test2",
                            WS_OVERLAPPEDWINDOW,
                            0,0,0,0, // 正确设置尺寸
                            nullptr, nullptr, GetModuleHandleW(nullptr),nullptr);
    SetWindowLongPtr(hw2, GWLP_USERDATA, (LONG_PTR)&strc);
    SetLayeredWindowAttributes(hw2, COLORREF(), NULL, LWA_COLORKEY);
    SetParent(hw2, hw1);
    SetWindowPos(hw2, HWND_TOP, 100,100,300,300, SWP_NOMOVE | SWP_NOSIZE);
    MoveWindow(hw2, 10,10, 100, 100, true);
  ::ShowWindow(hw1, SW_SHOW);
  ::UpdateWindow(hw1);
  ::ShowWindow(hw2, SW_SHOW);
  ::UpdateWindow(hw2);
  MSG msg;
  while (GetMessage(&msg, 0, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return 0;
}