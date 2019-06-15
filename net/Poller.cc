/*********************************************
* file:   Poller.cc
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-15 10:15:44
**********************************************/
#include "Poller.h"
#include "EventLoop.h"
#include "Channel.h"
#include <sys/poll.h>
#include <assert.h>
#include <iostream>
using namespace std;
using namespace tinyse::net;

Poller::Poller(EventLoop *loop) : m_ownerLoop(loop) { }

Poller::~Poller() { }

void Poller::poll(int timeoutMs, ChannelList *activeChannels) {
    int numEvents = ::poll(&*m_pollfds.begin(), m_pollfds.size(), timeoutMs);
    if(numEvents > 0) {
        cout << numEvents << " events hanppended" << endl;
        fillActiveChannels(numEvents, activeChannels);
    }
    else if(numEvents == 0) {
        cout << "nothing happended" << endl;
    }
    else {
        cout << "Error: Poller::poll()" << endl;
    }
}

void Poller::fillActiveChannels(int numEvents, ChannelList *activeChannels) {
    for(auto &it : m_pollfds) {
        if(it.revents > 0) {
            --numEvents; //避免做无用功
            auto ch = m_channels.find(it.fd);
            assert(ch != m_channels.end());
            Channel *channel = ch->second;
            assert(channel->fd() == it.fd);
            channel->setRevents(it.revents);
            activeChannels->push_back(channel);
        }
    } 
}

void Poller::updateChannel(Channel *channel) {
    cout << "Poller::updateChannel()" << endl;
    assertInLoopThread();
    cout << "Poller::2" << endl;
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
    cout << "Poller1::assertInLoopThread()" << endl;
    m_ownerLoop->assertInLoopThread();
    cout << "Poller2::assertInLoopThread()" << endl;
}

