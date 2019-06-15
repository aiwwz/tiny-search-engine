/*********************************************
* file:   EventLoop.cc
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-14 16:41:54
**********************************************/
#include "EventLoop.h"
#include "Channel.h"
#include "Poller.h"
#include <pthread.h>
#include <assert.h>
#include <sys/poll.h>
//#include "../include/tinyLogger.h"
#include <iostream>
using namespace tinyse;
using namespace tinyse::net;
using std::cout;  using std::endl;

//__thread变量每个线程有一份独立实体, 各个线程互不干扰
__thread EventLoop *p_eventLoopOfCurrentThread = nullptr;

EventLoop::EventLoop()
    : m_looping(false)
    , m_threadID(pthread_self())
    , m_poller(new Poller(this)){ 
    if(p_eventLoopOfCurrentThread) {
        cout << "Error: 该线程 " << m_threadID 
             << " 已存在其他EventLoop: " << p_eventLoopOfCurrentThread << endl;
        //logError("该线程:%d 已存在其他EventLoop: %d", m_threadID, p_eventLoopOfCurrentThread);

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
    cout << "EventLoop::assertInLoopThread()" << endl;
    if(!isInLoopThread()) {
        abortNotInLoopThread();
    }
}

bool EventLoop::isInLoopThread() const {
    return m_threadID == pthread_self();
}

EventLoop* EventLoop::getEventLoopOfCurrentThread() {
    return p_eventLoopOfCurrentThread ? p_eventLoopOfCurrentThread : nullptr;
}

void EventLoop::loop() {
    assert(!m_looping);
    assertInLoopThread();
    m_looping = true;
    m_quit = false;

    while(!m_quit) {
        m_activeChannels.clear();
        cout << "Before poll" << endl;
        m_poller->poll(10000, &m_activeChannels);
        cout << "After poll" << endl;
        for(auto &it : m_activeChannels) {
            it->handleEvent();
        }
    }
    cout << "EventLoop " << this << " stop looping" << endl;
    m_looping = false;
}

void EventLoop::abortNotInLoopThread() const {
    cout << "Abort: EventLoop " << this << " 在线程tid = " << m_threadID
         << " 中创建, 但当前线程是tid = " << pthread_self() << endl;
}

void EventLoop::updateChannel(Channel *channel) {
    cout << "EventLoop::updateChannel()" << endl;
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    m_poller->updateChannel(channel);
}

void EventLoop::quit() {
    m_quit = true;
    //wakeup();
}
