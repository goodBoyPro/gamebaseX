#ifndef GRIDWORLD_H
#define GRIDWORLD_H
#include <base/base.h>
#include <functional>
#include <mutex>
#include <set>
#include <shared_mutex>

template <class T> class ListSafe : public std::multiset<T*> {
private:
  std::vector<T *> invalidActors;

public:
  std::mutex mut;
  void addActor(T *a) {
    std::unique_lock lk(mut);
    std::multiset<T*>::insert(a);
  }
  void remove(T *ptr) {
    std::unique_lock lk(mut);
    std::multiset<T>::erase(ptr);
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
    left = 0,
    leftup,
    up,
    rightup,
    right,
    rightdown,
    down,
    leftdown
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
  std::multiset<T *, compare> actorsAlive;

public:
  GridMap() {}
  GridMap(FVector2 beginPoint_, int row_, int column_, float height_,
          float width_) {
    init(beginPoint_, row_, column_, height_, width_);
  }
  void init(FVector2 beginPoint_, int row_, int column_, float height_,
            float width_) {
    beginPoint = beginPoint_;
    row = row_;
    column = column_;
    height = height_;
    width = width_;

    gridmapNode<T *>::gridmapNodeWidth = width_;
    gridmapNode<T *>::gridmapNodeHeight = height_;
    edgeUp = beginPoint.y + height;
    edgeDown = beginPoint.y + height * (row - 1);
    edgeLeft = beginPoint.x + width;
    edgeRight = beginPoint.x + width * (column - 1);
    int num = row * column;
    allNode = new gridmapNode<T >[num];
    for (int i = 0; i < num; i++) {
      gridmapNode<T> &an = allNode[i];

      int x = i / column;
      int y = i % column;
      if (x == 0 || y == 0 || y == column - 1 || x == row - 1) {
        continue;
      }

      an.nodeNear[gridmapNode<T >::down] =
          i + column < num - row ? &allNode[i + column] : nullptr; // ok
      an.nodeNear[gridmapNode<T >::left] =
          (i - 1) % column != 0 ? &allNode[i - 1] : nullptr; // ok
      an.nodeNear[gridmapNode<T >::leftdown] =
          i + column - 1 < num - row && (i + column - 1) % column != 0
              ? &allNode[i + column - 1]
              : nullptr;
      an.nodeNear[gridmapNode<T >::leftup] =
          i - column - 1 > column - 1 && (i - column - 1) % column != 0
              ? &allNode[i - column - 1]
              : nullptr;
      an.nodeNear[gridmapNode<T >::right] =
          (i + 1) % column != column - 1 ? &allNode[i + 1] : nullptr; // ok
      an.nodeNear[gridmapNode<T >::rightdown] =
          i + column + 1 < num - row && (i + column + 1) % column != column - 1
              ? &allNode[i + column + 1]
              : nullptr;
      an.nodeNear[gridmapNode<T >::rightup] =
          i - column + 1 > column - 1 && (i - column + 1) % column != column - 1
              ? &allNode[i - column + 1]
              : nullptr;
      an.nodeNear[gridmapNode<T >::up] =
          i - column > column - 1 ? &allNode[i - column] : nullptr; // ok
    }
  }
  int getPositionIndex(const FVector3 &pos) {
    if (pos.x < edgeLeft || pos.x >= edgeRight //
        || pos.y < edgeUp || pos.y >= edgeDown) {
      return 0;
    }
    int column_ = (pos.x - beginPoint.x) / width;
    int row_ = (pos.y - beginPoint.y) / height;
    return row_ * column + column_;
  }
  std::vector<T *> badActors;//未处理
  int addActor(T *a) {
    int id = getPositionIndex(a->getPositionWs());
    if (id) {
      allNode[id].actors.addActor(a);
      return id;
    } else {
      printf("actor is out edge,been deleted");
      badActors.push_back(a);
      return 0;
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
    gridmapNode<T > &gridNode = allNode[centerId];

    gridNode.actors.pollList([&](T *a) { actorsAlive.insert(a); });
    for (auto elem : gridNode.nodeNear) {
      if (elem) {
        elem->actors.pollList([&](T *a) { actorsAlive.insert(a); });
      }
    }
  }
  void changeActorNode(T* ptr, int idNew, int idOld) {
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
  ~GridMap() { delete[] allNode; }
};

#endif // GRIDWORLD_H