/*********************************************
* file:   test1.cc
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-14 23:58:56
**********************************************/
#include <iostream>
#include "EventLoop.h"
#include <sys/types.h>
#include <unistd.h>
#include "../include/Thread.h"
using namespace std;

tinyse::net::EventLoop *g_loop;

void threadFunc() {
    g_loop->loop();
}

int main() {
    cout << "threadFunc(): pid = " << getpid()
         << ", tid = " << pthread_self() << endl;
    tinyse::net::EventLoop loop;
    g_loop = &loop;
    tinyse::Thread thread(threadFunc);
    thread.start();
    thread.join();

    return 0;
}
