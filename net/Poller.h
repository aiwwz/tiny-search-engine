/*********************************************
* file:   Poller.h
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-15 10:01:39
**********************************************/
#ifndef __POLLER_H__
#define __POLLER_H__
#include "Uncopyable.h"
#include <vector>
#include <map>

struct pollfd;
 
namespace tinyse {
namespace net {

class Channel;
class EventLoop;

/* 封装I/O多路复用epoll */
class Poller : Uncopyable {
    using ChannelList = std::vector<Channel*>;
    using PollFdList = std::vector<struct pollfd>;
    using ChannelMap = std::map<int, Channel*>;
public:
    Poller(EventLoop *loop);
    ~Poller();

    void poll(int timeoutMs, ChannelList *activeChannels);
    void updateChannel(Channel *channel);
    void removeChannel(Channel *channel);

    void assertInLoopThread();

private:
    void fillActiveChannels(int numEvents, ChannelList *activeChannels);

private:
    EventLoop *m_ownerLoop; //该Poller的所有者EventLoop
    PollFdList m_pollfds;
    ChannelMap m_channels;  //fd到Channel*的映射
};

} //end of namespace net
} //end of namespace tinyse
 
#endif /* __POLLER_H__ */
