/*********************************************
* file:   test3.cc
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-07-01 22:40:04
**********************************************/
#include <iostream>
#include <sys/timerfd.h>
#include <cstring>
#include "../include/EventLoop.h"
using namespace std;
using namespace tinyse;

EventLoop *g_loop;
void timeout() {
    cout << "main: Timeout!" << endl;
    g_loop->quit();
}
 
int main() {
    EventLoop loop;
    g_loop = &loop;
    
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    Channel channel(&loop, timerfd);
    channel.setReadCallback(timeout);
    channel.enableReading();

    struct itimerspec howlong;
    bzero(&howlong, sizeof(howlong));
    howlong.it_value.tv_sec = 5;
    ::timerfd_settime(timerfd, 0, &howlong, nullptr);
    
    loop.loop();
 
    return 0;
}
