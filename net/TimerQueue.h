/*********************************************
* file:   TimerQueue.h
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-15 19:01:29
**********************************************/
#ifndef __TIMERQUEUE_H__
#define __TIMERQUEUE_H__
#include <functional>
#include <memory>
#include <vector>
#include <set>
#include "Channel.h"
#include "Uncopyable.h"
#include "../include/Timestamp.h"

namespace tinyse {
namespace net {

using TimerCallback = std::function<void()>;

class EventLoop;

class Timer {
public:
    Timer(const TimerCallback &cb, Timestamp when, double interval)
        : m_timerCallback(cb)
        , m_expiration(when)
        , m_interval(interval)
        , m_repeat(interval > 0) { }
    
    void run() const {
        m_timerCallback();
    }

    Timestamp expiration() const {
        return m_expiration;
    }

    bool repeat() const {
        return m_repeat;
    }

    void restart(Timestamp now) {
        if(m_repeat) {
            m_expiration = addTime(now, m_interval);
        }
        else {
            m_expiration = Timestamp::invalid();
        }
    }

private:
    TimerCallback m_timerCallback;
    Timestamp m_expiration;
    const double m_interval;
    const bool m_repeat;
};


class TimerID {
public:
    TimerID(Timer *timer) : m_timer(timer) { } 
    
private:
    unique_ptr<Timer> m_timer;
};


class TimerQueue : Uncopyable {
    using Entry = std::pair<Timestamp, Timer*>; 
    using TimerList = std::set<Entry>;
public:
    TimerQueue(EventLoop *loop);
    ~TimerQueue();

    TimerID addTimer(const TimerCallback &cb, Timestamp when, double interval); 
    void cancel(TimerID timerID);

private:
    void handleRead();
    std::vector<Entry> getExpired(Timestamp now);
    void reset(const std::vector<Entry> &expird, Timestamp now);
    bool insert(Timer *timer);

private:
    EventLoop *m_loop;
    const int m_timerfd;
    TimerList m_timers;
    Channel m_timerfdChannel;
};

} //end of namespace net
} //end of namespace tinyse
 
#endif /* __TIMERQUEUE_H__ */
