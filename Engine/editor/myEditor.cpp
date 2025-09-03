#ifndef MYEDITOR_H
#define MYEDITOR_H
#include "worldEditor.h"
int main() {
  WorldEditorWindow window;
  //  window.window2.create(sf::VideoMode(400,400),"window2");
  // getUiManager().createBigWindow(L"win2",window.window2.getSystemHandle())->createWindow<MiniWindow>("winxxx");
 
  window.loop();
  
  return 0;}
#endif// MYEDITOR_H