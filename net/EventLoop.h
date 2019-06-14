/*********************************************
* file:   EventLoop.h
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-14 16:37:17
**********************************************/
#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__
#include "Uncopyable.h" 
#include "../include/Config.h"
#include <sys/types.h>

BEGIN_NAMESPACE_NET

class EventLoop {
public:
    EventLoop();
    ~EventLoop();

    void loop();
    void assertInLoopThread() const;
    bool isInLoopThread() const;

    static EventLoop* getEventLoopOfCurrentThread();

private:
    void abortNotInLoopThread() const;

private:
    bool m_looping;
    const pid_t m_threadID;
};

END_NAMESPACE_NET
 
#endif /* __EVENTLOOP_H__ */
