#include <Windows.h>
#include <string>
std::string wstring_to_utf8(const std::wstring &wstr);
std::string utf8_to_gbk(const std::string &utf8_str);
std::string gbkToUtf8(const std::string &gbk_str);
std::wstring utf8ToWstring(const std::string &utf8_str);
std::wstring gbkToWstring(const std::string &gbk_str);
std::string wstringToGbk(const std::wstring &wstr);
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
inline std::string wstring_to_utf8(const std::wstring &wstr) {
  if (wstr.empty())
    return "";
  // 计算转换所需的缓冲区大小
  int utf8_size = WideCharToMultiByte(CP_UTF8,          // 目标编码：UTF-8
                                      0,                // 转换标志：默认
                                      wstr.c_str(),     // 源宽字符串
                                      (int)wstr.size(), // 源字符串长度
                                      nullptr, // 目标缓冲区：先不填，仅计算大小
                                      0,       // 目标缓冲区大小：0 表示仅计算
                                      nullptr, nullptr // 无效字符替换（默认）
  );
  if (utf8_size <= 0)
    return ""; // 转换失败
  // 分配缓冲区并执行转换
  std::string utf8_str(utf8_size, 0);
  WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(),
                      &utf8_str[0], // 目标缓冲区
                      utf8_size, nullptr, nullptr);
  return utf8_str;
}
inline std::string utf8_to_gbk(const std::string &utf8_str) {
  // 步骤1：先将UTF-8转换为UTF-16（Windows宽字符，作为中间编码）
  // 先获取转换所需的宽字符缓冲区长度（包含终止符）
  int wide_char_len =
      MultiByteToWideChar(CP_UTF8,          // 源编码类型：UTF-8
                          0,                // 转换标志：0表示默认行为
                          utf8_str.c_str(), // 输入的UTF-8字符串
                          -1,      // 输入长度：-1表示自动计算（包含\0终止符）
                          nullptr, // 输出缓冲区：先传nullptr获取所需长度
                          0        // 输出缓冲区大小：0表示仅计算长度
      );
  if (wide_char_len == 0) {
    // 转换失败（可能是UTF-8格式无效或系统不支持）
    return "";
  }

  // 分配宽字符缓冲区并执行转换
  wchar_t *wide_buf = new (std::nothrow) wchar_t[wide_char_len];
  if (!wide_buf) {
    // 内存分配失败
    return "";
  }
  // 实际执行UTF-8到UTF-16的转换
  if (MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, wide_buf,
                          wide_char_len) == 0) {
    delete[] wide_buf;
    return "";
  }

  // 步骤2：将UTF-16（宽字符）转换为GBK编码（代码页936）
  // 先获取GBK缓冲区所需长度（包含终止符）
  int gbk_len =
      WideCharToMultiByte(936,      // 目标编码：GBK对应的代码页（936）
                          0,        // 转换标志：0表示默认行为
                          wide_buf, // 输入的UTF-16宽字符
                          -1,       // 输入长度：-1表示自动计算（包含\0终止符）
                          nullptr,  // 输出缓冲区：先传nullptr获取所需长度
                          0,        // 输出缓冲区大小：0表示仅计算长度
                          nullptr, nullptr // 不使用默认替换字符
      );
  if (gbk_len == 0) {
    delete[] wide_buf;
    return "";
  }

  // 分配GBK缓冲区并执行转换
  char *gbk_buf = new (std::nothrow) char[gbk_len];
  if (!gbk_buf) {
    delete[] wide_buf;
    return "";
  }
  // 实际执行UTF-16到GBK的转换
  if (WideCharToMultiByte(936, 0, wide_buf, -1, gbk_buf, gbk_len, nullptr,
                          nullptr) == 0) {
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
inline std::string gbkToUtf8(const std::string &gbk_str) {
  if (gbk_str.empty())
    return "";

  // 步骤1：GBK转宽字符（UTF-16）
  int wide_char_len = MultiByteToWideChar(936, // 源编码：GBK对应的代码页
                                          0,   // 转换标志：默认
                                          gbk_str.c_str(), // 输入GBK字符串
                                          -1, // 自动计算长度（包含终止符）
                                          nullptr, 0 // 先获取所需缓冲区大小
  );
  if (wide_char_len == 0)
    return "";

  wchar_t *wide_buf = new (std::nothrow) wchar_t[wide_char_len];
  if (!wide_buf)
    return "";
  if (MultiByteToWideChar(936, 0, gbk_str.c_str(), -1, wide_buf,
                          wide_char_len) == 0) {
    delete[] wide_buf;
    return "";
  }

  // 步骤2：宽字符转UTF-8（直接复用已有函数wstring_to_utf8）
  std::string utf8_str = wstring_to_utf8(std::wstring(wide_buf));
  delete[] wide_buf;
  return utf8_str;
}

// 2. UTF-8转宽字符（UTF-16）
inline std::wstring utf8ToWstring(const std::string &utf8_str) {
  if (utf8_str.empty())
    return L"";

  // 直接将UTF-8转换为宽字符（UTF-16）
  int wide_char_len = MultiByteToWideChar(CP_UTF8,          // 源编码：UTF-8
                                          0,                // 转换标志：默认
                                          utf8_str.c_str(), // 输入UTF-8字符串
                                          -1, // 自动计算长度（包含终止符）
                                          nullptr, 0 // 先获取所需缓冲区大小
  );
  if (wide_char_len == 0)
    return L"";

  wchar_t *wide_buf = new (std::nothrow) wchar_t[wide_char_len];
  if (!wide_buf)
    return L"";
  if (MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, wide_buf,
                          wide_char_len) == 0) {
    delete[] wide_buf;
    return L"";
  }

  // 构造wstring（自动忽略末尾的终止符）
  std::wstring wstr(wide_buf);
  delete[] wide_buf;
  return wstr;
}

// 3. GBK转宽字符（UTF-16）
inline std::wstring gbkToWstring(const std::string &gbk_str) {
  if (gbk_str.empty())
    return L"";

  // 直接将GBK转换为宽字符（UTF-16）
  int wide_char_len = MultiByteToWideChar(936, // 源编码：GBK对应的代码页
                                          0,   // 转换标志：默认
                                          gbk_str.c_str(), // 输入GBK字符串
                                          -1, // 自动计算长度（包含终止符）
                                          nullptr, 0 // 先获取所需缓冲区大小
  );
  if (wide_char_len == 0)
    return L"";

  wchar_t *wide_buf = new (std::nothrow) wchar_t[wide_char_len];
  if (!wide_buf)
    return L"";
  if (MultiByteToWideChar(936, 0, gbk_str.c_str(), -1, wide_buf,
                          wide_char_len) == 0) {
    delete[] wide_buf;
    return L"";
  }

  // 构造wstring（自动忽略末尾的终止符）
  std::wstring wstr(wide_buf);
  delete[] wide_buf;
  return wstr;
}
inline std::string wstringToGbk(const std::wstring &wstr) {
  if (wstr.empty())
    return "";

  // 步骤：直接将宽字符（UTF-16）转换为GBK编码（代码页936）
  // 先计算GBK缓冲区所需长度（包含终止符）
  int gbk_len = WideCharToMultiByte(936,          // 目标编码：GBK对应的代码页
                                    0,            // 转换标志：默认
                                    wstr.c_str(), // 输入宽字符串
                                    -1,           // 自动计算长度（包含终止符）
                                    nullptr, 0,   // 先获取所需缓冲区大小
                                    nullptr, nullptr);
  if (gbk_len == 0)
    return ""; // 转换失败

  // 分配GBK缓冲区并执行转换
  char *gbk_buf = new (std::nothrow) char[gbk_len];
  if (!gbk_buf)
    return ""; // 内存分配失败

  if (WideCharToMultiByte(936, 0, wstr.c_str(), -1, gbk_buf, gbk_len, nullptr,
                          nullptr) == 0) {
    delete[] gbk_buf;
    return ""; // 转换失败
  }

  // 构造结果字符串（自动忽略末尾的\0终止符）
  std::string gbk_str(gbk_buf);

  // 清理动态分配的内存
  delete[] gbk_buf;

  return gbk_str;
}