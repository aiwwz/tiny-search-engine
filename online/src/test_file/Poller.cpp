/*********************************************
* file:   Poller.cc
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-15 10:15:44
**********************************************/
#include <sys/poll.h>
#include <assert.h>
#include <iostream>
#include "../include/Poller.hpp"
#include "../include/EventLoop.h"
#include "../include/Channel.h"
#include "../include/MyLogger.h"
using namespace std;
using namespace tinyse;

Poller::Poller(EventLoop *loop) : m_ownerLoop(loop) { }

Poller::~Poller() { }

Timestamp Poller::poll(int timeoutMs, ChannelList *activeChannels) {
    //获得当前活动事件
    int numEvents = ::poll(&*m_pollfds.begin(), m_pollfds.size(), timeoutMs);

    Timestamp now(Timestamp::now());

    if(numEvents > 0) {
        LogInfo("%d events happened!", numEvents);
        //遍历m_pollfds, 找出所有活动事件fd, 将其对应的Channel填入activeChannels
        fillActiveChannels(numEvents, activeChannels);
    }
    else if(numEvents == 0) {
        LogInfo("timeout: nothing happended!");
    }
    else {
        LogError("poll()");
    }

    return now;
}

/* 遍历m_pollfds, 找出所有活动事件fd, 将其对应的Channel填入activeChannels */
void Poller::fillActiveChannels(int numEvents, ChannelList *activeChannels) {
    for(auto pfd = m_pollfds.begin(); pfd != m_pollfds.end() && numEvents > 0; ++pfd) {
        if(pfd->revents > 0) {
            --numEvents; //避免做无用功
            auto ch = m_channels.find(pfd->fd);
            assert(ch != m_channels.end());
            Channel *channel = ch->second;
            assert(channel->fd() == pfd->fd);
            channel->setRevents(pfd->revents);
            activeChannels->push_back(channel);
        }
    } 
}

/* 维护和更新m_poolfds数组 */
void Poller::updateChannel(Channel *channel) {
    assertInLoopThread();
    if(channel->index() < 0) { //添加新Channel
        LogInfo("add new fd = %d, events = %d", channel->fd(), channel->events());
        assert(m_channels.find(channel->fd()) == m_channels.end());
        struct pollfd pfd;
        pfd.fd = channel->fd();
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        m_pollfds.push_back(pfd); //添加新的带监听fd
        int idx = static_cast<int>(m_pollfds.size()) - 1;
        channel->setIndex(idx);
        m_channels[pfd.fd] = channel;
    }
    else { //更新已有Channel
        LogInfo("renew fd = %d, events = %d", channel->fd(), channel->events());
        assert(m_channels.find(channel->fd()) != m_channels.end());
        assert(m_channels[channel->fd()] == channel);
        int idx = channel->index();
        assert(idx >= 0 && idx < static_cast<int>(m_pollfds.size()));
        struct pollfd &pfd = m_pollfds[idx]; //通过下标可以在O(1)内更新已有Channel
        assert(pfd.fd == channel->fd() || pfd.fd == -channel->fd()-1);
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        if(channel->isNoneEvent()) {
            pfd.fd = -channel->fd() - 1; //忽略该pollfd(且防止fd为0)
        }
    }
}

void Poller::removeChannel(Channel *channel) {
    LogInfo("fd = %d", channel->fd());
    assertInLoopThread();
    assert(m_channels.find(channel->fd()) != m_channels.end());
    assert(m_channels[channel->fd()] == channel);
    assert(channel->isNoneEvent());
    int idx = channel->index();
    assert(idx >= 0 && idx < static_cast<int>(m_pollfds.size()));
    const struct pollfd &pfd = m_pollfds[idx]; (void)pfd; //防编译warn
    assert(pfd.fd == -channel->fd()-1 && pfd.events == channel->events());
    size_t n = m_channels.erase(channel->fd()); (void)n; //防编译warn
    assert(n == 1);

    //优化到O(1)复杂度, 防止删除元素时拷贝
    if(static_cast<size_t>(idx) == m_pollfds.size() - 1) {
        m_pollfds.pop_back();
    }
    else { //要删除元素非末尾元素, 则将待删除的元素与最后一个元素交换
        int backFd = m_pollfds.back().fd;
        iter_swap(m_pollfds.begin() + idx, m_pollfds.end() - 1);
        if(backFd < 0) {
            backFd = -backFd - 1;
        }
        m_channels[backFd]->setIndex(idx); //更新channel
        m_pollfds.pop_back();
    }
}

void Poller::assertInLoopThread() {
    m_ownerLoop->assertInLoopThread();
}

