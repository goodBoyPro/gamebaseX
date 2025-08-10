#ifndef GAMELOG_H
#define GAMELOG_H
#include <cstdio>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <stdio.h>
#include <string>
#include <thread>

class GameLog {
  std::string logfileName;
  std::ofstream ofile;
  std::string folder = "logs/";
  std::string logString;
  std::string buffer;
  std::thread *thread = nullptr;
  std::mutex mut;
  bool bRun = true;

public:
  ~GameLog() {
    bRun = false;
    thread->join();
    ofile.close();
    delete thread;
  }
  GameLog() {
    initFolder();
    std::string filename = folder + get_current_date();
    ofile.open(filename);
    thread = new std::thread(&GameLog::mainLoop, this);
  }
  void log(const std::string &str) {
    printf("%s", str.c_str());
    std::unique_lock lk(mut);
    logString.append(str).append("\n");
  }
  void saveLog() {
    std::unique_lock lk(mut);
    std::swap(buffer, logString);

    logString.resize(0);
    lk.unlock();
    ofile.write(buffer.data(), buffer.size());
    buffer.resize(0);
  }
  static GameLog &getGameLog() {
    static GameLog inst;
    return inst;
  }
  std::string get_current_date() {
    std::time_t now = std::time(nullptr);
    std::tm *local_tm = std::localtime(&now); // 注意：非线程安全

    char buffer[40];
    std::snprintf(buffer, sizeof(buffer),
                  "log%04d_%02d_%02d_%02d_%02d_%02d.txt",
                  local_tm->tm_year + 1900, // 年份从1900开始计算
                  local_tm->tm_mon + 1,     // 月份0-11需+1
                  local_tm->tm_mday,        //
                  local_tm->tm_hour,        //
                  local_tm->tm_min,         //
                  local_tm->tm_sec);        // 日期1-31

    return std::string(buffer);
  }
  void initFolder() {
    if (!std::filesystem::exists(folder)) {
      std::filesystem::create_directory(folder);
    }
  }
  void mainLoop() {
    while (bRun) {
      static auto lastSaveTime = std::chrono::steady_clock::now();
      // 设置保存间隔（示例设置为5秒）
      constexpr auto SAVE_INTERVAL = std::chrono::seconds(5);
      auto currentTime = std::chrono::steady_clock::now();
      auto elapsed = currentTime - lastSaveTime;
      // 超过间隔时间则保存
      if (elapsed >= SAVE_INTERVAL) {
        saveLog();
        lastSaveTime = currentTime; // 重置计时器
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }
};
#endif // GAMELOG_H