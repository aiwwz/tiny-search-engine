/*********************************************
* file:   test3.cc
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-15 16:42:39
**********************************************/
#include <iostream>
#include <sys/timerfd.h>
#include <cstring>
#include <unistd.h>
#include "EventLoop.h"
#include "Channel.h"
using namespace std;

tinyse::net::EventLoop *g_loop;

void timeout() {
    cout << "Timeout!" << endl;
    g_loop->quit();
}
 
int main() {
    tinyse::net::EventLoop loop;
    g_loop = &loop;
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    cout << "1111111111" << endl;
    tinyse::net::Channel channel(&loop, timerfd);
    channel.setReadCallback(timeout);
    cout << "1111155555" << endl;
    channel.enableReading();
    cout << "2222222222" << endl;

    struct itimerspec howlong;
    bzero(&howlong, sizeof(howlong));
    howlong.it_value.tv_sec = 5;
    ::timerfd_settime(timerfd, 0, &howlong, NULL);
    cout << "3333333333" << endl;

    loop.loop();
    cout << "4444444444" << endl;
    ::close(timerfd);
    cout << "5555555555" << endl;
 
    return 0;
}
