#include "heads.h"
class GController {
   private:
    struct Cbk {
        Cbk(const std::function<void()> cbk, int handle) {
            function = cbk;
            id = handle;
        }
        std::function<void()> function;
        int id = 0;
    };

   public:
    enum Input { onKeybordA, onKeybordD, onKeybordW, onKeybordS, count };
    std::vector<std::vector<Cbk>> cbks;
    GController() {
        for (int i = 0; i < (int)count; i++) {
            cbks.push_back(std::vector<std::function<void()>>());
        }
    }
    int bind(Input eInput, const std::function<void()>& cbk) {
        cbks[eInput].push_back(cbk);
        return cbks[eInput].size() - 1;
    }
    void unBind(Input eInput, int handle) {
        cbks[eInput][handle] = []() {};
    }
    void loop(const sf::RenderWindow& window, const sf::Event& event) {
        if (event.key.code == sf::Keyboard::A) {
            for (const std::function<void()>& cbk : cbks[onKeybordA]) {
                cbk();
            }
        }
        if (event.key.code == sf::Keyboard::D) {
            for (const std::function<void()>& cbk : cbks[onKeybordD]) {
                cbk();
            }
        }
        if (event.key.code == sf::Keyboard::W) {
            for (const std::function<void()>& cbk : cbks[onKeybordW]) {
                cbk();
            }
        }
        if (event.key.code == sf::Keyboard::S) {
            for (const std::function<void()>& cbk : cbks[onKeybordS]) {
                cbk();
            }
        }
    }
};