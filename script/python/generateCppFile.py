import json
class cppdata:
    CONTROLLER_DATA=R'''#ifndef  CONTROLLER_H
#define CONTROLLER_H
#include "heads.h"
class GController {
  private:
    int cbkId = 0;
    struct Cbk {
        Cbk(const std::function<void()> cbk, int handle) {
            function = cbk;
            id = handle;
        }
        std::function<void()> function;
        int id = 0;
    };
  public:
    enum Input {
      ENUM_HOLDER_PLACE,count
    };
    std::vector<std::vector<Cbk>> cbks;
    GController() {
        for (int i = 0; i < (int)count; i++) {
            cbks.push_back(std::vector<Cbk>());
        }
    }
    int bind(Input eInput, const std::function<void()> &cbk) {
        cbkId++;
        cbks[eInput].emplace_back(cbk, cbkId);
        return cbkId;
    }
    void unBind(Input eInput, int handle) {
        auto &vec = cbks[eInput];
        vec.erase(std::remove_if(vec.begin(), vec.end(), [handle](const Cbk &cbk) { return cbk.id == handle; }), vec.end());
    }
    void executeCbk(Input eInput) {
        std::vector<Cbk> &cbks_ = cbks[eInput];
        for (Cbk &cbk : cbks_) {
            cbk.function();
        }
    }
    void loop(sf::RenderWindow &window, sf::Event &event) {
        LINEAR_HODER_PLACE
        // 点击时
        while (window.pollEvent(event)) {
            // 关闭窗口事件
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed) {
                CLICK_HODER_PLACE
            }
        }
    }
};
#endif
'''
def openAndWrite(str,filepath):
    with open(filepath,'w',encoding='utf8')as file:
        file.write(str)
def generate_CONTROLLER_H():
    with open("inputConfig.json",'r',encoding='utf8')as file:
        data=json.load(file)
        enums=[]
        for v in data.values():
           for k in v.keys():
               enums.append(k)
        enum_holder=','.join(enums)

        linears=[]
        for k,v in data['linear'].items():
            linears.append(f"if(sf::Keyboard::isKeyPressed({v})) {{executeCbk({k});}}")
        linearsHolder=''.join(linears)
        
        clicks=[]
        for k,v in data['click'].items():
            clicks.append(f" if (event.key.code == {v}) {{for (Cbk &cbk : cbks[{k}]) {{cbk.function();}}}}")
        clicksHolder=''.join(clicks)
    str=cppdata.CONTROLLER_DATA.replace("ENUM_HOLDER_PLACE",enum_holder)
    str=str.replace("LINEAR_HODER_PLACE",linearsHolder)
    str=str.replace("CLICK_HODER_PLACE",clicksHolder)
    openAndWrite(str,"build/generatedCodeFile/controller.h")
    

        

generate_CONTROLLER_H()

