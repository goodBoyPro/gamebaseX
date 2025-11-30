#ifndef SOURCEREFER_H
#define SOURCEREFER_H
#include <cassert>
#include <utility>
class SourceIF;
template <class T> class SourceRefer {
private:
  T *ptr = nullptr;

public:
  T *getPtr() const { return ptr; }
  friend class SourceIF;
  T *operator->() {
    // assert(ptr != nullptr && "Dereferencing null pointer");
    return ptr;
  };
  const T *operator->() const {
    if (ptr == nullptr) {
      // assert(false && "Dereferencing null pointer in const operator->()");
    }
    return ptr;
  }
  void *operator&() = delete;
  void *operator&() const = delete;
  SourceRefer() {
    static_assert(std::is_base_of_v<SourceIF, T>,
                  "T must be a subclass of SourceIF");
  };
  virtual ~SourceRefer() {
    if (ptr) {
      ptr->referCount--;
    }
  };
  SourceRefer(const SourceRefer &sr) {
    ptr = sr.ptr;
    ptr->referCount++;
  };
  SourceRefer(SourceRefer &&sr) {
    ptr = sr.ptr;
    sr.ptr = nullptr;
  };
  void *operator=(const SourceRefer &sr) {
    if (ptr) {
      ptr->referCount--;
    }
    ptr = sr.ptr;
    ptr->referCount++;
    return this;
  };
  void *operator=(SourceRefer &&sr) {
    if (ptr) {
      ptr->referCount--;
    }
    ptr = sr.ptr;
    sr.ptr = nullptr;
    return this;
  };
  int getReferCount() const {
    if (ptr) {
      return ptr->referCount;
    } else {
      return 0;
    }
  };
  void releaseSrc() {
    if (ptr) {
      delete ptr;
      ptr = nullptr;
    }
  }
};
class SourceIF {
public:
  template <class T> friend class SourceRefer;
  int referCount = 0;
  virtual ~SourceIF() {};
  template <class T> SourceRefer<T> makeRefer() {
    SourceRefer<T> ref;
    ref.ptr = (T *)this;
    ref.ptr->referCount++;
    return ref;
  }
};
#endif // SOURCEREFER_H