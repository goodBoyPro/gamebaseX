#include<framework.h>
void GActor::loop(float deltatime,GameWindow&window){
      for(GComponent*comp:__allComponents){
         comp->loop();
      }
   }
