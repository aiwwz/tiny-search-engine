/*********************************************
* file:   Thread.h
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-09 21:55:19
**********************************************/
#ifndef __THREAD_H__
#define __THREAD_H__
#include <pthread.h>
#include <functional>

namespace tinyse {

/* 封装线程pthread */
class Thread {
    using ThreadFunc = std::function<void()>;
public:
    Thread(ThreadFunc &&func) 
        : m_started(false)
        , m_pthid(0) 
        , m_tid(0) 
        , m_func(move(func)) {}

    virtual ~Thread();

    void start();
    void join();
    pid_t tid() const;

private:
    static void* threadFunc(void*);
private:
    bool m_started;
    pthread_t m_pthid;
    pid_t m_tid;
    ThreadFunc m_func;
};

} //end of namespace tinyse

#endif /* __THREAD_H__ */
