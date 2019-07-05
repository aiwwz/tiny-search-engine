/*********************************************
* file:   Thread.h
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-09 21:55:19
**********************************************/
#ifndef __THREAD_H__
#define __THREAD_H__
#include <pthread.h>
#include <functional>
#include <string>
using std::function;  using std::string;

namespace tinyse {

/* 封装线程pthread */
class Thread {
    using ThreadFunc = function<void()>;
public:
    Thread(ThreadFunc &&func, const string &name = string()) 
        : m_started(false)
        , m_pthid(0) 
        , m_tid(0) 
        , m_func(move(func)) 
        , m_name(name) { }

    virtual ~Thread();

    void start();
    void join();
    pid_t tid() const { return m_tid; }
    const string& name() const { return m_name; }
    bool started() const { return m_started; }

private:
    static void* threadFunc(void*);
    
private:
    bool m_started; //是否开始运行
    pthread_t m_pthid;
    pid_t m_tid;
    ThreadFunc m_func; //执行任务的回调函数
    string m_name;  //此线程的名字(for logging)
};

} //end of namespace tinyse

#endif /* __THREAD_H__ */
