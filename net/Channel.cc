/*********************************************
* file:   Channel.cc
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-14 16:27:07
**********************************************/
#include <sys/epoll.h> 
#include "Channel.h"
#include "EventLoop.h"
using namespace tinyse::net;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI; 
const int Channel::kWriteEvent = EPOLLOUT;

void Channel::update() {
    m_loop
}
