#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H
#include <functional>
#include <vector>
#include <stdio.h>
#include "chrono"
class TimeManager {
  private:
    struct Task {

        uint64_t absTime;
        int handle = 0;
        int delayMs;
        int times = 1;
        bool isValid = true;
        bool bInfinite = false;
        std::function<void()> cbk;
        Task() = default;
        Task(int handle_, int delayMs_, uint64_t absTime_, int times_, const std::function<void()> &cbk_, bool bInfinite_) {
            handle = handle_;
            delayMs = delayMs_;
            absTime = absTime_;
            times = times_;
            cbk = std::move(cbk_);
            bInfinite = bInfinite_;
        }
        void operator()() {
            if (times <= 0) {
                isValid = false;
                return;
            }
            cbk();
            absTime = absTime + delayMs;
            times--;
        }
    };

  private:
    std::vector<Task> allTasks;
    uint64_t absTime = 0;
    int handle = 1;

  public:
    TimeManager() {
        auto dur = std::chrono::system_clock::now().time_since_epoch();
        absTime = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
    }
    int addTask(int delayMs_, int times_, const std::function<void()> cbk_, bool bInfinite = false) {
        handle++;
        allTasks.emplace_back(handle, delayMs_, absTime + delayMs_, times_, cbk_, bInfinite);
        return handle;
    }
    void cancelTask(int handle_) {
        for (Task &task : allTasks) {
            if (task.handle == handle_) {
                task.isValid = false;
                return;
            }
        }
    }
    int clearFlag = 0;
    void loop() {
        auto dur = std::chrono::system_clock::now().time_since_epoch();
        absTime = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
        for (Task &task : allTasks) {
            if (absTime >= task.absTime)
                task();
        }
        // allTasks.erase(std::remove_if(allTasks.begin(), allTasks.end(), [](const Task &task) { return !task.isValid; }), allTasks.end());
        clearFlag++;
        if (clearFlag == 1000) {
            auto endit = std::remove_if(allTasks.begin(), allTasks.end(), [](const Task &task) { return !task.isValid; });
            allTasks.resize(std::distance(allTasks.begin(), endit));
            clearFlag=0;
        }
    }
    int getAlltaskCount() { return allTasks.size(); }
};
class TaskOnce {};

#endif // TIMERMANAGER_H