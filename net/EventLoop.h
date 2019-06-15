/*********************************************
* file:   EventLoop.h
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-14 16:37:17
**********************************************/
#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__
#include "Uncopyable.h" 
//#include "../include/CurrentThread.h" //for gettid()
#include "../include/Config.h"
#include <sys/types.h>
#include <vector>
#include <memory>

BEGIN_NAMESPACE_NET

class Channel;
class Poller;

class EventLoop {
    using ChannelList = std::vector<Channel*>;
public:
    EventLoop();
    ~EventLoop();

    void loop();
    void assertInLoopThread() const;
    bool isInLoopThread() const;
    static EventLoop* getEventLoopOfCurrentThread();
    void updateChannel(Channel *channel);
    void quit();

private:
    void abortNotInLoopThread() const;

private:
    bool m_looping;
    bool m_quit;
    const  pthread_t m_threadID;
    std::unique_ptr<Poller> m_poller;
    ChannelList m_activeChannels;
};

END_NAMESPACE_NET
 
#endif /* __EVENTLOOP_H__ */
