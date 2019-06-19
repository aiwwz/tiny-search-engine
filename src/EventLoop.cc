/*********************************************
* file:   EventLoop.cc
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-14 16:41:54
**********************************************/
#include "../include/EventLoop.h"
#include "../include/Channel.h"
#include "../include/Poller.h"
#include <pthread.h>
#include <assert.h>
#include <sys/poll.h>
//#include "../include/tinyLogger.h"
#include <iostream>
#include <sys/eventfd.h>
using namespace tinyse;
using namespace tinyse::net;
using std::cout;  using std::endl;

//__thread变量每个线程有一份独立实体, 各个线程互不干扰
__thread EventLoop *p_eventLoopOfCurrentThread = nullptr;

static int createEventfd() {
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if(evtfd < 0) {
        perror("EventLoop::eventfd");
        abort();
    }
    return evtfd;
}

EventLoop::EventLoop()
    : m_looping(false)
    , m_threadID(pthread_self())
    , m_poller(new Poller(this))
    , m_timerQueue(new TimerQueue(this))
    , m_wakeupFd(createEventfd())
    , m_wakeupChannel(new Channel(this, m_wakeupFd)){ 
    if(p_eventLoopOfCurrentThread) {
        cout << "Error: 该线程 " << m_threadID 
             << " 已存在其他EventLoop: " << p_eventLoopOfCurrentThread << endl;
        //logError("该线程:%d 已存在其他EventLoop: %d", m_threadID, p_eventLoopOfCurrentThread);

    }
    else {
        p_eventLoopOfCurrentThread = this;
    }
    m_wakeupChannel->setReadCallback(std::bind(&EventLoop::handleRead, this));
    m_wakeupChannel->enableReading();
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
        m_pollRuntime = m_poller->poll(10000, &m_activeChannels);
        for(auto &it : m_activeChannels) {
            it->handleEvent();
        }
        doPendingFunctors();
    }
    cout << "EventLoop " << this << " stop looping" << endl;
    m_looping = false;
}

void EventLoop::abortNotInLoopThread() const {
    cout << "Abort: EventLoop " << this << " 在线程tid = " << m_threadID
         << " 中创建, 但当前线程是tid = " << pthread_self() << endl;
}

void EventLoop::updateChannel(Channel *channel) {
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    m_poller->updateChannel(channel);
}

void EventLoop::quit() {
    m_quit = true;
    if(!isInLoopThread()) {
        wakeup();
    }
}

void EventLoop::handleRead() {
    uint64_t one;
    ssize_t n = ::read(m_wakeupFd, &one, sizeof one);
    if (n != sizeof one) {
        perror("EventLoop: read");       
    }
}

void EventLoop::doPendingFunctors() {
    std::vector<Functor> functors;
    m_callingPendingFuntors = true;
    {
        MutexLockGuard lock(m_mutex);
        functors.swap(m_pendingFunctors);
    }

    for(auto &cb : functors) {
        cb();
    }
    m_callingPendingFuntors = false;
}
