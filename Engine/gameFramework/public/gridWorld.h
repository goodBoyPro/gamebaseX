#ifndef GRIDWORLD_H
#define GRIDWORLD_H
#include <base/base.h>
#include <functional>
#include <mutex>
#include <set>
#include <shared_mutex>

template <class T> class ListSafe : public std::multiset<T *> {
private:
  std::vector<T *> invalidActors;

public:
  std::mutex mut;
  void addActor(T *a) {
    std::unique_lock lk(mut);
    std::multiset<T *>::insert(a);
  }
  void remove(T *ptr) {
    std::unique_lock lk(mut);
    std::multiset<T *>::erase(ptr);
  }
  void pollList(std::function<void(T *)> callback) {
    std::unique_lock lk(mut);
    for (auto it = this->begin(); it != this->end();) {
      if ((!(*it)->isValid)) {
        invalidActors.push_back(*it);
        it = this->erase(it);
        continue;
      }
      callback(*it);
      it++;
    }
    for (T *ptr : invalidActors) {
      delete ptr;
    }
    invalidActors.clear();
  }
};

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
template <class T> struct gridmapNode {
  inline static float gridmapNodeWidth;
  inline static float gridmapNodeHeight;
  enum nodeName {
    up,
    rightUp,
    right,
    rightDown,
    down,
    leftDown,
    left,
    leftUp,
    count
  };
  gridmapNode *nodeNear[8] = {nullptr};
  ListSafe<T> actors;
  FVector2 point;
  bool contain(const FVector3 &pos) {
    return pos.x >= point.x && pos.x < point.x + gridmapNodeWidth &&
           pos.y >= point.y && pos.y < point.y + gridmapNodeHeight;
  }
  void clear() {
    for (auto elem : actors) {
      delete elem;
    }
    actors.clear();
  }
  ~gridmapNode() { clear(); }
};
//////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

template <class T> class GridMap {
private:
  struct compare {
    bool operator()(const T *a, const T *b) const {
      return a->getPositionWs().y < b->getPositionWs().y;
    }
  };

public:
  FVector2 beginPoint;
  int row;
  int column;
  float height;
  float width;
  float edgeLeft;
  float edgeRight;
  float edgeUp;
  float edgeDown;
  gridmapNode<T> *allNode;
  //   std::multiset<T *, compare> actorsAlive;
  std::vector<T *> actorsAlive;

public:
  GridMap() {}
  GridMap(int row_, int column_, float height_, float width_) {
    init(row_, column_, height_, width_);
  }
  void init(int row_, int column_, float height_, float width_) {

    row = row_;
    column = nextPowerOfTwo(column_);
    height = height_;
    width = width_;
    beginPoint = {-row / 2.f * height, -column / 2.f * width};
    printf("beginPoint:%f,%f\n", beginPoint.x, beginPoint.y);

    gridmapNode<T>::gridmapNodeWidth = width_;
    gridmapNode<T>::gridmapNodeHeight = height_;
    edgeUp = beginPoint.y;
    edgeDown = beginPoint.y + height * (row);
    edgeLeft = beginPoint.x;
    edgeRight = beginPoint.x + width * (column);
    setNodes();
  }
  void setNodes() {
    int num = row * column;
    allNode = new gridmapNode<T>[num];
    int dr[] = {-1, -1, 0, 1, 1, 1, 0, -1};
    int dc[] = {0, 1, 1, 1, 0, -1, -1, -1};
    for (int i = 0; i < num; i++) {
      gridmapNode<T> &an = allNode[i];
      int qRow = i / column;
      int qColumn = i % column;
      an.point = {beginPoint.x + qColumn * gridmapNode<T>::gridmapNodeWidth,
                  beginPoint.y + qRow * gridmapNode<T>::gridmapNodeHeight};
      for (int j = 0; j < gridmapNode<T>::count; j++) {
        int r = qRow + dr[j];
        int c = qColumn + dc[j];
        if (r < 0 || r >= row || c < 0 || c >= column) {
          allNode[i].nodeNear[j] = nullptr;
        } else {
          allNode[i].nodeNear[j] = &(allNode[r * column + c]);
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
  int getPositionIndex(const FVector3 &pos) {
    if (pos.x < edgeLeft || pos.x >= edgeRight //
        || pos.y < edgeUp || pos.y >= edgeDown) {
      return -1;
    }
    int column_ = (pos.x - beginPoint.x) / width;
    int row_ = (pos.y - beginPoint.y) / height;
    return row_ * column + column_;
  }
  std::vector<T *> badActors;
  int addActor(T *a) {
    int id = getPositionIndex(a->getPositionWs());
    if (id != -1) {
      allNode[id].actors.addActor(a);
      return id;
    } else {
      printf("%s is out edge,been deleted\n", a->getGClass().className.c_str());
      badActors.push_back(a);
      return -1;
    }
  }
  bool areFloatsEqual(const float a, const float b, float epsilon = 1e-5f) {
    return std::fabs(a - b) < epsilon;
  }
  bool areFloatsLessThan(float a, float b, float epsilon = 1e-6f) {
    return a < (b - epsilon);
  }
  void setActorsAlive(int centerId) {

    for (auto elem : badActors)
      delete elem;
    badActors.clear();
    gridmapNode<T> &gridNode = allNode[centerId];
    actorsAlive.resize(0);
    gridNode.actors.pollList([&](T *a) { actorsAlive.push_back(a); });
    for (auto elem : gridNode.nodeNear) {
      if (elem) {
        elem->actors.pollList([&](T *a) { actorsAlive.push_back(a); });
      }
    }
  }
  void changeActorNode(T *ptr, int idNew, int idOld) {
    if (idOld == -1)
      return;

    allNode[idOld].actors.remove(ptr);

    allNode[idNew].actors.addActor(ptr);
  }
  int getActorsNumber() {
    int a = 0;
    int num = row * column;
    for (int i = 0; i < num; i++) {
      a += allNode[i].actors.size();
    }

    return a;
  }
  std::vector<T*> getAllActors() {
    std::vector<T*>all;
    int num = row * column;
    for (int i = 0; i < num; i++) {
      for (auto a : allNode[i].actors) {
        all.push_back(a);
      };
    }
    return all;
  }
  ~GridMap() {
    delete[] allNode;
    for (auto elem : badActors)
      delete elem;
  }
};

#endif // GRIDWORLD_H