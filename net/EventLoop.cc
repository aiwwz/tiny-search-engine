/*********************************************
* file:   EventLoop.cc
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-14 16:41:54
**********************************************/
#include "CurrentThread.h" //for gettid()
#include "EventLoop.h"
#include <pthread.h>
#include <assert.h>
#include <sys/poll.h>
#include <iostream>
using namespace tinyse;
using namespace tinyse::net;
using std::cout;  using std::endl;

//__thread变量每个线程有一份独立实体, 各个线程互不干扰
__thread EventLoop *p_eventLoopOfCurrentThread = nullptr;

EventLoop::EventLoop()
    : m_looping(false)
    , m_threadID(gettid()) { 
    if(p_eventLoopOfCurrentThread) {
        cout << "Error: 该线程 " << m_threadID 
             << " 已存在其他EventLoop" << p_eventLoopOfCurrentThread << endl;
    }
    else {
        p_eventLoopOfCurrentThread = this;
    }
}

EventLoop::~EventLoop() {
    assert(!m_looping);
    p_eventLoopOfCurrentThread = nullptr; //当前线程中已不存在EventLoop
}

void EventLoop::assertInLoopThread() const {
    if(!isInLoopThread()) {
        abortNotInLoopThread();
    }
}

bool EventLoop::isInLoopThread() const {
    return m_threadID == gettid();
}

EventLoop* EventLoop::getEventLoopOfCurrentThread() {
    return p_eventLoopOfCurrentThread ? p_eventLoopOfCurrentThread : nullptr;
}

void EventLoop::loop() {
    assert(!m_looping);
    assertInLoopThread();
    m_looping = true;
    poll(NULL, 0, 5*1000); //测试: do nothing!

    m_looping = false;
}
