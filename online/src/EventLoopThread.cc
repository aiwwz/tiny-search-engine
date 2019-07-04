/*********************************************
* file:   EventLoopThread.cc
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-07-02 09:40:04
**********************************************/
#include "../include/EventLoopThread.h"
#include "../include/EventLoop.h"
#include <cassert>
using namespace tinyse;

EventLoopThread::EventLoopThread(const ThreadInitCallback &cb) 
    : m_loop(nullptr) 
    , m_mutex()
    , m_cond(m_mutex)
    , m_thread(std::bind(&EventLoopThread::threadFunc, this)) 
    , m_exit(false)
    , m_cb(cb) { }

EventLoopThread::~EventLoopThread() {
    m_exit = true;
    if(m_loop != nullptr) {
        m_loop->quit();
        m_thread.join();
    }
}

/* EventLoopThread启动自己的线程, 并在其中运行loop() */
EventLoop* EventLoopThread::startLoop() {
    assert(!m_thread.started());
    m_thread.start();

    {
        MutexLockGuard lock(m_mutex);
        while(m_loop == nullptr) { //防止虚假唤醒
            m_cond.wait(); //等待线程的创建与运行
        }
    }

    return m_loop; //返回新线程中的EventLoop对象
}

void EventLoopThread::threadFunc() {
    EventLoop loop; //在栈上定义EventLoop对象

    {
        MutexLockGuard lock(m_mutex);
        m_loop = &loop;
        m_cond.signal(); //唤醒startLoop()
    }

    loop.loop();

    {
        MutexLockGuard lock(m_mutex);
        m_loop = nullptr;
    }
}
