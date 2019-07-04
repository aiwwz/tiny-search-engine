/*********************************************
* file:   EventLoopThread.h
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-07-02 09:38:31
**********************************************/
#ifndef __EVENTLOOPTHREAD_H__
#define __EVENTLOOPTHREAD_H__
#include <string>
#include "MutexLock.h"
#include "Condition.h"
#include "Thread.h"
#include "Uncopyable.h"
using std::string;
 
namespace tinyse {

class EventLoop; 

class EventLoopThread : Uncopyable {
    using ThreadInitCallback = std::function<void(EventLoop*)>; 
public:
    EventLoopThread(const ThreadInitCallback &cb = ThreadInitCallback());
    ~EventLoopThread();
    EventLoop* startLoop();

private:
    void threadFunc();

private:
    EventLoop *m_loop;
    MutexLock m_mutex;
    Condition m_cond;
    Thread m_thread;
    bool m_exit;
    ThreadInitCallback m_cb;
};

} //end of namespace tinyse
 
#endif /* __EVENTLOOPTHREAD_H__ */
