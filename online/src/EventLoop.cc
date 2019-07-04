/*********************************************
* file:   EventLoop.cc
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-14 16:41:54
**********************************************/
#include "../include/EventLoop.h"
#include "../include/Channel.h"
#include "../include/Epoller.h"
#include "../include/MyLogger.h"
#include <pthread.h>
#include <assert.h>
#include <sys/poll.h>
#include <iostream>
#include <sys/eventfd.h>
#include <signal.h>
using namespace tinyse;
using std::cout;  using std::endl;

//__thread变量每个线程有一份独立实体, 各个线程互不干扰
__thread EventLoop *p_eventLoopOfCurrentThread = nullptr;

static int createEventfd() { //创建wakeupfd
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if(evtfd < 0) {
        perror("EventLoop::eventfd");
        abort();
    }
    return evtfd;
}

class IgnoreSigPipe {
public:
    IgnoreSigPipe() {
        ::signal(SIGPIPE, SIG_IGN);
    }
};

IgnoreSigPipe initObj; //忽略SIGPIPE

EventLoop::EventLoop()
    : m_looping(false)
    , m_threadID(pthread_self())
    , m_poller(new Epoller(this))
    , m_timerQueue(new TimerQueue(this))
    , m_wakeupFd(createEventfd())
    , m_wakeupChannel(new Channel(this, m_wakeupFd)) {

    if(p_eventLoopOfCurrentThread) {
        LogError("Another EventLoop %d exists in this thread %d", p_eventLoopOfCurrentThread, m_threadID);
        exit(-1);
    }
    else {
        p_eventLoopOfCurrentThread = this;
    }

    //注册wakeupChannel
    m_wakeupChannel->setReadCallback(std::bind(&EventLoop::handleRead, this));
    m_wakeupChannel->enableReading();
}

EventLoop::~EventLoop() {
    LogDebug("EventLoop %d of thread %d destructs in thread %d", this, m_threadID, pthread_self());
    m_wakeupChannel->disableAll();
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
    return p_eventLoopOfCurrentThread; //如果当前线程不是IO线程的话, 返回nullptr
}

void EventLoop::loop() {
    assert(!m_looping);
    assertInLoopThread(); //确保事件循环必须在IO线程执行
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

    LogInfo("EventLoop %d stop looping!", this);
    m_looping = false;
}

void EventLoop::abortNotInLoopThread() const {
    cout << "Abort: EventLoop " << this << " created in thread = " << m_threadID
         << ", but current thread is " << pthread_self() << endl;
    exit(-1);
}

void EventLoop::updateChannel(Channel *channel) {
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    m_poller->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel) {
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    //...
    m_poller->removeChannel(channel);
}

void EventLoop::quit() { //设置结束标志
    m_quit = true;
    if(!isInLoopThread()) {
        wakeup(); //唤醒EventLoop
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
