#ifndef MYEDITOR_H
#define MYEDITOR_H
#include "worldEditor.h"
int main() {
  WorldEditorWindow window;
  getUiManager().createBigWindow(L"win2",window.window.getSystemHandle())->createWindow<MiniWindow>("winxxx");
 
  window.loop();
  
  return 0;}
#endif// MYEDITOR_H