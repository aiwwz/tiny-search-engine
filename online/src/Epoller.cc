/*********************************************
* file:   Epoller.cc
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-07-04 16:32:05
**********************************************/
#include "../include/Epoller.h"
#include "../include/EventLoop.h"
#include "../include/Channel.h"
#include "../include/MyLogger.h"
#include <sys/epoll.h>
#include <assert.h>
#include <cstring>

const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;

using namespace tinyse;

Epoller::Epoller(EventLoop *loop) 
    : m_ownerLoop(loop)
    , m_epfd(::epoll_create1(EPOLL_CLOEXEC))
    , m_events(kInitEventListSize) {
        
    if(m_epfd < 0) {
        perror("Epoller::epoll_create1");
    }
}

Epoller::~Epoller() {
    ::close(m_epfd);
}

Timestamp Epoller::poll(int timeoutMs, ChannelList *activeChannels) {
    int numEvents = ::epoll_wait(m_epfd, &*m_events.begin(), static_cast<int>(m_events.size()), timeoutMs);
    Timestamp now(Timestamp::now());
    if(numEvents > 0) {
        LogInfo("%d events happended", numEvents);
        fillActiveChannels(numEvents, activeChannels);
        if(static_cast<size_t>(numEvents) == m_events.size()) {
            m_events.resize(m_events.size() * 2);
        }
    }
    else if (numEvents == 0) {
        LogInfo("timeout: nothing happended!");
    }
    else {
        perror("Epoller::poll::epoll_wait");
    }
    return now;
}

/* 将m_events中的活动fd填入activeChannels中 */
void Epoller::fillActiveChannels(int numEvents, ChannelList *activeChannels) const {
    assert(static_cast<size_t>(numEvents) <= m_events.size());
    for(int idx = 0; idx < numEvents; ++idx) {
        Channel *channel = static_cast<Channel*>(m_events[idx].data.ptr);

        int fd = channel->fd();
        ChannelMap::const_iterator it = m_channels.find(fd); (void)it;
        assert(it != m_channels.end());
        assert(it->second == channel);

        channel->setRevents(m_events[idx].events);
        activeChannels->push_back(channel);
    }
}

void Epoller::updateChannel(Channel *channel) {
    assertInLoopThread();
    const int index = channel->index();
    if(index == kNew || index == kDeleted) { //添加或者删除Channel
        int fd = channel->fd();
        if(index == kNew) {
            LogInfo("add new fd = %d, events = %d", channel->fd(), channel->events());
            assert(m_channels.find(fd) == m_channels.end());
            m_channels[fd] = channel;
        }
        else {
            LogInfo("delete fd = %d, events = %d", channel->fd(), channel->events());
            assert(m_channels.find(fd) != m_channels.end());
            assert(m_channels[fd] == channel);
        }
        channel->setIndex(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }
    else { //更新已有Channel
        LogInfo("renew fd = %d, events = %d", channel->fd(), channel->events());
        int fd = channel->fd(); (void)fd;
        assert(m_channels.find(fd) != m_channels.end());
        assert(m_channels[fd] == channel);
        assert(index == kAdded);
        if(channel->isNoneEvent()) {
            update(EPOLL_CTL_DEL, channel);
            channel->setIndex(kDeleted);
        }
        else {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void Epoller::removeChannel(Channel *channel) {
    assertInLoopThread();
    int fd = channel->fd();
    LogInfo("fd = %d", fd);
    assert(m_channels.find(fd) != m_channels.end());
    assert(m_channels[fd] == channel);
    assert(channel->isNoneEvent());
    int index = channel->index();
    assert(index == kAdded || index == kDeleted);
    size_t n = m_channels.erase(fd); (void)n;
    assert(n == 1);
    if(index == kAdded) {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->setIndex(kNew);
}

void Epoller::update(int operation, Channel *channel) {
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.events = channel->events();
    ev.data.ptr = channel;
    int fd = channel->fd();
    if(::epoll_ctl(m_epfd, operation, fd, &ev) != 0) {
        perror("Epoller::update::epoll_ctl");
    }
}

void Epoller::assertInLoopThread() {
    m_ownerLoop->assertInLoopThread();
}
