/*********************************************
 * file:   test6.cc
 * author: AIWWZ(wzj1524@qq.com)
 * date:   2019-07-02 10:35:09
 **********************************************/
#include "../include/EventLoop.h"
#include "../include/EventLoopThread.h"
#include <stdio.h>
#include <iostream>
using namespace tinyse;
using namespace std;

void runInThread() {
    printf("runInThread(): pid = %d, tid = %ld\n", getpid(), pthread_self());
}

int main() {
    printf("main(): pid = %d, tid = %ld\n", getpid(), pthread_self());

    EventLoopThread loopThread;
    EventLoop* loop = loopThread.startLoop();
    loop->runInLoop(runInThread);
    sleep(1);

    loop->runAfter(2, runInThread);
    sleep(3);

    loop->quit();
    printf("exit main().\n");
}

