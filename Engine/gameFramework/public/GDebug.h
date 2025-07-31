#if !defined(GDBUG)
#define GDBUG
// #define _CRT_NON_CONFORMING_SWPRINTFS
#include <base/base.h>
#include<GComman.h>
#include <mutex>
#include <set>
#define STRINGSIZE 128
#define PRINTDEBUG(...)                                                        \
    {                                                                          \
        static GDebug __g__;                                                   \
        swprintf(__g__.wchar_,STRINGSIZE-1, __VA_ARGS__);                                   \
    }                                                                          \
                                                                               \
// 已实现线程安全
class GDebug{
  private:
    void drawLog(sf::RenderWindow&window_,int index);
    static std::mutex mtxDebugs___;
  public:
    wchar_t wchar_[STRINGSIZE] = {0};
    GDebug();
    virtual ~GDebug();
    static void debugDisplay(sf::RenderWindow&window_);
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

inline std::set<GDebug *> debugs__;
inline std::mutex GDebug::mtxDebugs___;

inline void GDebug::debugDisplay(sf::RenderWindow&window_)
{
    // 非必要操作，应避免在堆区创建对象；
    //  debugs__.erase(std::remove_if(debugs__.begin(), debugs__.end(),
    //                                [](GDebug *a) { return a->shouldDestroy; }),debugs__.end());
    std::unique_lock lk(mtxDebugs___);
    int i = 0;
    for (auto a : debugs__)
    {   
        if(a==nullptr)continue;
        a->drawLog(window_,i);
        i++;
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline GDebug::GDebug()
{
    debugs__.emplace(this);
}

inline GDebug::~GDebug() {  
    std::unique_lock lk(mtxDebugs___);
   debugs__.erase(this);//可能存在线程安全问题，遇到时解决//与clearDebugs()冲突
}

inline void GDebug::drawLog(sf::RenderWindow&window_,int index)
{
    printText(window_,wchar_, 0, 30 * index);
}
 
 

#endif // GDBUG
