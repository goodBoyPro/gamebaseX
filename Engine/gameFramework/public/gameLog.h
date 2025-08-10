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
#define GAMELOG(string) GameLog::getGameLog().log(string) 
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
    std::string filename = folder + setLogFileName();
    ofile.open(filename);
    thread = new std::thread(&GameLog::mainLoop, this);
  }
  void log(const std::string &str_) {
    const std::string str=getCurrentTime().append(":").append(str_);
    printf("%s\n", str.c_str());
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
  std::string setLogFileName() {
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

    return buffer;
  }
  std::string getCurrentTime() {
    std::time_t now = std::time(nullptr);
    std::tm *local_tm = std::localtime(&now); // 注意：非线程安全

    char buffer[40];
    std::snprintf(buffer, sizeof(buffer),
                  "%04d%02d%02d:%02d:%02d:%02d",
                  local_tm->tm_year + 1900, // 年份从1900开始计算
                  local_tm->tm_mon + 1,     // 月份0-11需+1
                  local_tm->tm_mday,        //
                  local_tm->tm_hour,        //
                  local_tm->tm_min,         //
                  local_tm->tm_sec);        // 日期1-31

    return buffer;
  }
  void initFolder() {
    if (!std::filesystem::exists(folder)) {
      std::filesystem::create_directory(folder);
    }
  }
  void mainLoop() {
    while (bRun) {
      saveLog();
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
  }
};
#endif // GAMELOG_H