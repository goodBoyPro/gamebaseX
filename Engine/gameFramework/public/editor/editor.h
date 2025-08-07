#ifndef  EDITOR_H
#define EDITOR_H
#include<base/base.h>
#include<vector>
class EditorWindow{
    public:
    virtual~EditorWindow(){}
    virtual void loop(){}
};
class GWorldEditor{
    public:
    GameWindow window;
    GWorldEditor(){window.create(sf::VideoMode(800,600), "WorldEditor");}
};
class GEditor{
    std::vector<EditorWindow*>allWindow;
    GEditor(){}
    void loop(){
        for(EditorWindow*childWindow:allWindow){
            childWindow->loop();
        }
    }
};
#endif// EDITOR_H