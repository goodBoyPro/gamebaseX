#ifndef SOURCEREFER_H
#define SOURCEREFER_H
#include <cassert>
#include <utility> 
class SourceIF;
template <class T>
class SourceRefer {
   private:
    T* ptr = nullptr;

   public:
    friend class SourceIF;
    T* operator->() {
        assert(ptr != nullptr && "Dereferencing null pointer");
        return ptr;
    };
    void* operator&() = delete;
    void* operator&() const = delete;
    SourceRefer() {
        static_assert(std::is_base_of_v<SourceIF, T>, "T must be a subclass of SourceIF");
    };
    virtual~SourceRefer() {
        if (ptr) {
            ptr->referCount--;
        }
    };
    SourceRefer(const SourceRefer& sr) {
        ptr = sr.ptr;
        ptr->referCount++;
    };
    SourceRefer(SourceRefer&& sr) {
        ptr = sr.ptr;
        sr.ptr = nullptr;
    };
    void* operator=(const SourceRefer& sr) {
        if (ptr) {
            ptr->referCount--;
        }
        ptr = sr.ptr;
        ptr->referCount++;
        return this;
    };
    void* operator=(SourceRefer&& sr) {
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
};
class SourceIF {
   public:
   
    template <class T>
    friend class SourceRefer;
    int referCount = 0;
    virtual ~SourceIF() {};
    //实现时必须调用initReferCount()创建引用管理器
    virtual SourceRefer<SourceIF> loadFromFile(const char* filePath){SourceRefer<SourceIF> ref;return ref;}; 
    SourceRefer<SourceIF>  initReferCount(){
        SourceRefer<SourceIF> ref;
        ref.ptr = this;
        ref.ptr->referCount++;
        return ref;
    }
};
#endif  // SOURCEREFER_H