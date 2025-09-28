#ifndef SCRIPT_EXECUTOR_H
#define SCRIPT_EXECUTOR_H
#include "functional"
#include "string"
#include "thread"
#include "vector"
#include <iostream>
#include<fstream>
#include<mutex>
class ScriptExecutorManager {
public:
  ScriptExecutorManager() {};
  ~ScriptExecutorManager() {
    if (th.joinable()) {
      th.join();
    }
  };
  void
  registerExecutor(const std::string &name,
                   std::function<void(std::vector<std::string>)> executor) {
    executors[name] = executor;
  };

  void execute(const std::string &script) {
    const std::vector<std::string> &strs = getStrs(script);
    if (strs.empty()) {
      return;
    }
    const std::string &command = strs[0];
    auto it = executors.find(command);
    if (it != executors.end()) {
      it->second(std::vector<std::string>(strs.begin() + 1, strs.end()));
    }else{
        std::cerr << "invalid command: " << command << std::endl;
    }
  };
  std::thread th;
  std::mutex mut;
  void loop() {
    th = std::thread([this]() {
      while (true) {
        std::string script;
        std::cout << "Enter script command (or 'exit' to quit): ";
        std::getline(std::cin, script);
        if (script == "exit") {
          break;
        }
        std::lock_guard<std::mutex> lock(mut);
        execute(script);
        
      }
    });
  };
  void executeScriptFile(const std::string&path){
    std::ifstream file(path);
    if (!file.is_open()) {
      std::cerr << "Failed to open script file: " << path << std::endl;
      return;
    }
    std::string line;
    while (std::getline(file, line)) {
      execute(line);
    }
  }
static bool isNumber(const std::string& s) {
    // 空字符串不是数字
    if (s.empty()) {
        std::cerr << "不是有效数字" << std::endl;
        return false;
    }
    
    size_t start = 0;
    bool hasDecimalPoint = false;
    
    // 处理负号
    if (s[0] == '-') {
        start = 1;
        // 只有负号不是数字
        if (start >= s.size()) {
            std::cerr << "arg:"<<s<<" is not number" << std::endl;
            return false;
        }
    }
    
    // 检查数字部分
    for (size_t i = start; i < s.size(); ++i) {
        if (s[i] == '.') {
            // 不允许多个小数点或小数点在末尾
            if (hasDecimalPoint || i == s.size() - 1) {
                std::cerr << "arg:"<<s<<" is not number" << std::endl;
                return false;
            }
            hasDecimalPoint = true;
        }
        // 检查是否为数字字符
        else if (!std::isdigit(static_cast<unsigned char>(s[i]))) {
            std::cerr << "arg:"<<s<<" is not number" << std::endl;
            return false;
        }
    }
    
    // 所有检查通过，是有效数字
    return true;
}
private:
  std::unordered_map<std::string, std::function<void(std::vector<std::string>)>>
      executors;
 
std::vector<std::string> getStrs(const std::string &script) {
    std::vector<std::string> result;
    std::string current_token;  // 存储当前正在构建的词

    for (char c : script) {
        // 检查是否为字母、数字或下划线
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || 
            (c >= '0' && c <= '9') || c == '_'||c=='.'||c=='-') {
            current_token += c;  // 符合条件的字符添加到当前词
        } else {
            // 遇到其他字符时，若当前有累积的词则添加到结果
            if (!current_token.empty()) {
                result.push_back(current_token);
                current_token.clear();
            }
        }
    }

    // 处理字符串结束时剩余的词
    if (!current_token.empty()) {
        result.push_back(current_token);
    }

    return result;
}
  
};
#endif // SCRIPT_EXECUTOR_H