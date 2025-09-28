#include "scriptExecutor.h"
int main() {
  ScriptExecutorManager manager;
  manager.registerExecutor("print", [](const std::vector<std::string> &strs) {
    if (strs.size() == 3 &&                         //
        ScriptExecutorManager::isNumber(strs[1]) && //
        ScriptExecutorManager::isNumber(strs[2])    //
    ) {
      std::cout << strs[0] << strs[1] <<strs[2] << std::endl;
    }
  });
  manager.loop();
  while (1) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  return 0;
}