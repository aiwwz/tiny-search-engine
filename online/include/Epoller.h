/*********************************************
* file:   Epoller.h
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-15 10:01:39
**********************************************/
#ifndef __EPOLLER_H__
#define __EPOLLER_H__
#include "Uncopyable.h"
#include "../include/Timestamp.h"
#include <vector>
#include <map>
using std::vector;  using std::map;

struct epoll_event;
 
namespace tinyse {

class Channel;
class EventLoop;

/* 封装I/O多路复用epoll */
class Epoller : Uncopyable {
    using ChannelList = vector<Channel*>;
    using EventList = vector<struct epoll_event>;
    using ChannelMap = map<int, Channel*>;
public:
    Epoller(EventLoop *loop);
    ~Epoller();

    Timestamp poll(int timeoutMs, ChannelList *activeChannels);
    void updateChannel(Channel *channel);
    void removeChannel(Channel *channel);

    void assertInLoopThread();

private:
    static const int kInitEventListSize = 16;
    void fillActiveChannels(int numEvents, ChannelList *activeChannels) const;
    void update(int operation, Channel *channel);

private:
    EventLoop *m_ownerLoop; //该Poller的所有者EventLoop
    int m_epfd;
    EventList m_events;
    ChannelMap m_channels;  //fd到Channel*的映射
};

} //end of namespace tinyse
 
#endif /* __EPOLLER_H__ */
