#ifndef MYEDITOR_H
#define MYEDITOR_H
#include "editorFrame.h"
int main() {
  GEditor editor;
  editor.construct(200,100,"main");
  editor.mainloop();
  
  return 0;}
#endif// MYEDITOR_H