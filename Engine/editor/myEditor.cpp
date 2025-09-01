#ifndef MYEDITOR_H
#define MYEDITOR_H
#include "worldEditor.h"
int main() {
  WorldEditorWindow window;
  window.init();
  window.loop();
  
  return 0;}
#endif// MYEDITOR_H