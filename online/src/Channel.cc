/*********************************************
* file:   Channel.cc
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-14 16:27:07
**********************************************/
#include <sys/poll.h> 
#include "../include/Channel.h"
#include "../include/EventLoop.h"
#include "../include/MyLogger.h"
#include <iostream>
#include <cassert>
using namespace std;
using namespace tinyse;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI; 
const int Channel::kWriteEvent = POLLOUT;

Channel::~Channel() {
    assert(!m_eventHandling);
}

void Channel::update() {
   m_loop->updateChannel(this);
}

/*
void Channel::remove() {
    assert(isNoneEvent());
}
*/

void Channel::handleEvent() {
    m_eventHandling = true;

    if(m_revents & POLLNVAL) {
        LogWarn("Channel::handleEvent() POLLNVAL");
    }

    if((m_revents & POLLHUP) && !(m_revents & POLLIN)) {
        if(m_closeCallback) {
            m_closeCallback();
        }
    }

    if(m_revents & (POLLERR | POLLNVAL)) {
        if(m_errorCallback) {
            m_errorCallback();
        }
    }

    if(m_revents & (POLLIN | POLLPRI | POLLRDHUP)) {
        if(m_readCallback) {
            m_readCallback();
        }
    }
    
    if(m_revents & POLLOUT) {
        if(m_writeCallback) {
            m_writeCallback();
        }
    }
    
    m_eventHandling = false;
}
