/*********************************************
 * file:   TimerQueue.cc
 * author: AIWWZ(wzj1524@qq.com)
 * date:   2019-06-15 22:16:15
 **********************************************/
#include "TimerQueue.h"
#include "EventLoop.h"
#include <unistd.h>
#include <sys/timerfd.h>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cassert>
using namespace tinyse;
using namespace tinyse::net;
using std::cout; using std::endl;

int createTimerfd() {
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if(timerfd < 0) {
        perror("TimerQueue.cc:timerfd_create");
    }
    return timerfd;
}

struct timespec howMuchTimeFromNow(Timestamp &when) {
    int64_t microseconds = when.microSecondsSinceEpoch() - Timestamp::now().microSecondsSinceEpoch();
    if (microseconds < 100) {
        microseconds = 100;
    }
    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(microseconds / Timestamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<long>((microseconds % Timestamp::kMicroSecondsPerSecond) * 1000);
    return ts;
}

void readTimerfd(int timerfd, Timestamp now) {
    uint64_t howmany = 1;
    ssize_t n = ::read(timerfd, &howmany, sizeof(howmany));
    cout  << "TimerQueue::readTimerfd " << howmany << " at " << now.toString() << endl;
    if (n != sizeof howmany) {
        cout << "TimerQueue::handleRead() reads " << n << " bytes instead of 8";
    }
}

void resetTimerfd(int timerfd, Timestamp expiration) {
    struct itimerspec newValue;
    bzero(&newValue, sizeof newValue);
    newValue.it_value = howMuchTimeFromNow(expiration);
    int ret = ::timerfd_settime(timerfd, 0, &newValue, nullptr);
    if (ret){
        perror("timerfd_settime()");
    }
}

TimerQueue::TimerQueue(EventLoop *loop) 
    : m_loop(loop)
    , m_timerfd(createTimerfd())
    , m_timers()
    , m_timerfdChannel(loop, m_timerfd) {
    //设置timer回调
    m_timerfdChannel.setReadCallback(std::bind(&TimerQueue::handleRead, this));
    m_timerfdChannel.enableReading();
}

TimerQueue::~TimerQueue() {
    ::close(m_timerfd);
}

TimerID TimerQueue::addTimer(const TimerCallback &cb, Timestamp when, double interval) {
    Timer *timer = new Timer(cb, when, interval);
    m_loop->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
    cout << "------" << endl;
    return TimerID(timer);
}

void TimerQueue::addTimerInLoop(Timer* timer) {
    m_loop->assertInLoopThread();
    bool earliestChanged = insert(timer);
    if (earliestChanged) {
        resetTimerfd(m_timerfd, timer->expiration());
    }
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now) {
    std::vector<Entry> expired;
    Entry sentry = std::make_pair(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
    auto it = m_timers.lower_bound(sentry); //it指向第一个未到期Timer
    assert(it == m_timers.end() || now < it->first);
    std::copy(m_timers.begin(), it, back_inserter(expired)); //获取已到期Timer
    m_timers.erase(m_timers.begin(), it); //移除已到期Timer
    return expired;
}

void TimerQueue::reset(const std::vector<Entry> &expired, Timestamp now) {
    Timestamp nextExpire;
    for(auto &it : expired) {
        if (it.second->repeat()) {
            it.second->restart(now);
            insert(it.second);
        }
        else {
            // FIXME move to a free list
            delete it.second;
        }
    }

    if (!m_timers.empty()) {
        nextExpire = m_timers.begin()->second->expiration();
    }

    if (nextExpire.valid()) {
        resetTimerfd(m_timerfd, nextExpire);
    }
}

void TimerQueue::handleRead() {
    m_loop->assertInLoopThread();
    Timestamp now(Timestamp::now());
    readTimerfd(m_timerfd, now);
    std::vector<Entry> expired = getExpired(now);
    cout << "expired.size() = " << expired.size() << endl;
    for(auto &it : expired) {
        it.second->run(); 
    }
    reset(expired, now);
}

bool TimerQueue::insert(Timer* timer) {
    bool earliestChanged = false;
    Timestamp when = timer->expiration();
    TimerList::iterator it = m_timers.begin();
    if (it == m_timers.end() || when < it->first) {
        earliestChanged = true;
    }
    auto result = m_timers.insert(std::make_pair(when, timer));
    assert(result.second);
    return earliestChanged;
}
