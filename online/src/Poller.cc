/*********************************************
* file:   Poller.cc
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-15 10:15:44
**********************************************/
#include <sys/poll.h>
#include <assert.h>
#include <iostream>
#include "../include/Poller.h"
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
    cout << "fd = " << channel->fd() << " events = " << channel->events() << endl;
    if(channel->index() < 0) { //添加新Channel
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
        assert(m_channels.find(channel->fd()) != m_channels.end());
        assert(m_channels[channel->fd()] == channel);
        int idx = channel->index();
        assert(idx >= 0 && idx < static_cast<int>(m_pollfds.size()));
        struct pollfd &pfd = m_pollfds[idx]; //通过下标可以在O(1)内更新已有Channel
        assert(pfd.fd == channel->fd() || pfd.fd == -1);
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        if(channel->isNoneEvent()) {
            pfd.fd = -1; //忽略该pollfd
        }
    }
}

void Poller::assertInLoopThread() {
    m_ownerLoop->assertInLoopThread();
}

