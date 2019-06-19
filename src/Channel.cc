/*********************************************
* file:   Channel.cc
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-14 16:27:07
**********************************************/
#include <sys/poll.h> 
#include "../include/Channel.h"
#include "../include/EventLoop.h"
#include <iostream>
using namespace tinyse::net;
using namespace std;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI; 
const int Channel::kWriteEvent = POLLOUT;

void Channel::update() {
   m_loop->updateChannel(this); 
}

void Channel::handleEvent() {
    if(m_revents & POLLNVAL) {
        cout << "Channel::handleEvent() POLLNVAL" << endl;
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
}
