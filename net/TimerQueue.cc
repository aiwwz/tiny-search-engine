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

void readTimerfd(int timerfd, Timestamp &now) {
    uint64_t howmany;
    ssize_t n = ::read(timerfd, &howmany, sizeof howmany);
    cout  << "TimerQueue::handleRead() " << howmany << " at " << now.toString();
    if (n != sizeof howmany) {
        cout << "TimerQueue::handleRead() reads " << n << " bytes instead of 8";
    }
}

void resetTimerfd(int timerfd, Timestamp expiration) {
    struct itimerspec newValue;
    struct itimerspec oldValue;
    bzero(&newValue, sizeof newValue);
    bzero(&oldValue, sizeof oldValue);
    newValue.it_value = howMuchTimeFromNow(expiration);
    int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
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
    Timer *timer(new Timer(cb, when, interval));
    m_loop->assertInLoopThread();

    if(insert(timer)) {
        resetTimerfd(m_timerfd, timer->expiration());
    }
    return TimerID(timer);
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now) {
    std::vector<Entry> expired;
    Entry sentry = std::make_pair(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
    auto it = m_timers.lower_bound(sentry);
    assert(it == m_timers.end() || now < it->first);
    std::copy(m_timers.begin(), it, back_inserter(expired));
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
    std::pair<TimerList::iterator, bool> result = m_timers.insert(std::make_pair(when, timer));
    assert(result.second);
    return earliestChanged;
}
