#ifndef MYEDITOR_H
#define MYEDITOR_H
#include "worldEditor.h"
int main(int c,char**v) {
  auto program = getEditorProgram();
  // program.projectPath=v[1];

  program.mainLoop();
  return 0;}
#endif// MYEDITOR_H