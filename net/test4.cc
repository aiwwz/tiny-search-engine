/*********************************************
 * file:   test4.cc
 * author: AIWWZ(wzj1524@qq.com)
 * date:   2019-06-15 23:28:11
 **********************************************/
#include <iostream>
#include "EventLoop.h"
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "../include/CurrentThread.h"
#include "../include/Timestamp.h"
using namespace std;

int cnt = 0;
tinyse::net::EventLoop *g_loop;
void printTid() {
    printf("pid = %d, tid = %d\n", getpid(), tinyse::gettid());
    printf("now %s\n", tinyse::Timestamp::now().toString().c_str());
}

void print(const char* msg) {
    printf("msg %s %s\n", tinyse::Timestamp::now().toString().c_str(), msg);
    if (++cnt == 20) {
        g_loop->quit();
    }
}

int main() {
    printTid();
    tinyse::net::EventLoop loop;
    g_loop = &loop;

    print("main");
    loop.runAfter(1, std::bind(print, "once1"));
    loop.runAfter(1.5, std::bind(print, "once1.5"));
    loop.runAfter(2.5, std::bind(print, "once2.5"));
    loop.runAfter(3.5, std::bind(print, "once3.5"));
    loop.runEvery(2, std::bind(print, "every2"));
    loop.runEvery(3, std::bind(print, "every3"));

    loop.loop();
    print("main loop exits");
    sleep(1);
    return 0;
}
