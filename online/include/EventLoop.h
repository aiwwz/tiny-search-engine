/*********************************************
* file:   EventLoop.h
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-14 16:37:17
**********************************************/
#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__
#include "Uncopyable.h" 
#include "TimerQueue.h"
#include "../include/MutexLock.h"
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <memory>

namespace tinyse {

class Channel;
class Epoller;

class EventLoop {
    using ChannelList = std::vector<Channel*>;
    using Functor = std::function<void()>;
public:
    EventLoop();
    ~EventLoop();

    void loop();
    void assertInLoopThread() const;
    bool isInLoopThread() const;
    static EventLoop* getEventLoopOfCurrentThread();
    void updateChannel(Channel *channel);
    void removeChannel(Channel *channel);
    void quit();

    void runAt(const Timestamp &time, const TimerCallback &cb) {
        m_timerQueue->addTimer(cb, time, 0);
    }

    void runAfter(double delay, const TimerCallback &cb) {
        Timestamp time(addTime(Timestamp::now(), delay));
        runAt(time, cb);
    }

    void runEvery(double interval, const TimerCallback &cb) {
        Timestamp time(addTime(Timestamp::now(), interval));
        m_timerQueue->addTimer(cb, time, interval);
    }


    void wakeup() {
        uint64_t one = 1;
        ssize_t n = ::write(m_wakeupFd, &one, sizeof(one));
        if(n != sizeof(one)) {
            perror("EventLoop:write");
        }
    }

    void queueInLoop(const Functor &cb) {
        {
            MutexLockGuard lock(m_mutex);
            m_pendingFunctors.push_back(cb);        
        }
        if(!isInLoopThread() || m_callingPendingFuntors) {
            wakeup();
        }
    }

    void runInLoop(const Functor &cb) {
        if(isInLoopThread()) {
            cb();
        }
        else {
            queueInLoop(cb);
        }
    }

private:
    void abortNotInLoopThread() const;
    void handleRead(); //for wakeup
    void doPendingFunctors();

private:
    bool m_looping;
    bool m_quit;
    bool m_callingPendingFuntors;
    const  pthread_t m_threadID;
    Timestamp m_pollRuntime;
    std::unique_ptr<Epoller> m_poller; //间接持有poller
    std::unique_ptr<TimerQueue> m_timerQueue;
    ChannelList m_activeChannels;
    int m_wakeupFd;
    unique_ptr<Channel> m_wakeupChannel;
    MutexLock m_mutex;
    std::vector<Functor> m_pendingFunctors;
};

} //end of namespace tinyse
 
#endif /* __EVENTLOOP_H__ */
