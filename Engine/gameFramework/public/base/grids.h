#ifndef GRIDS_H
#define GRIDS_H
#include <stdexcept>
class Gquad {
public:
  enum { up, rightUp, right, rightDown, down, leftDown, left, leftUp, count };
  Gquad *quadNear[8] = {nullptr};

public:
  void setQuadNear() {}
};
class GGrid {
  int rows;
  int columns;
  Gquad *allQuads;

public:
  GGrid(int row_, int column_,float quadSize_,float beginX_,float beginY_) {
    if (row_ <= 0 || column_ <= 0) {
      throw std::range_error("invalid row or columns\n");
    }
    rows = row_;
    columns = nextPowerOfTwo(column_);
    setQuads();
  }
  void setQuads() {
    int num = rows * columns;
    allQuads = new Gquad[num];
    int dr[] = {-1, -1, 0, 1, 1, 1, 0, -1};
    int dc[] = {0, 1, 1, 1, 0, -1, -1, -1};

    for (int i = 0; i < num; i++) {
      int qRow = i / columns;
      int qColumn = i % columns;
      for (int j = 0; j < Gquad::count; j++) {
        int r = qRow + dr[j];
        int c = qColumn + dc[j];
        if (r < 0 || r >= rows || c < 0 || c >= columns) {
          allQuads[i].quadNear[j] = nullptr;
        } else {
          allQuads[i].quadNear[j] = &(allQuads[r * columns + c]);
        }
      }
    }
  }
  int nextPowerOfTwo(int n) {
    if (n <= 0)
      return 1; // 处理非正输入
    if ((n & (n - 1)) == 0)
      return n; // 如果已经是2的幂，直接返回

    // 找到最高位并左移一位
    int highestBit = 1;
    while (highestBit <= n) {
      highestBit <<= 1;
    }
    return highestBit;
  }
};
#endif // GRIDS_H