/*********************************************
* file:   test1.cc
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-28 18:21:03
**********************************************/
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include "../include/EventLoop.h"
#include "../include/Thread.h"
using namespace std;
using namespace tinyse;

void threadFunc() {
    printf("threadFunc(): pid = %d, tid = %ld\n", getpid(), pthread_self());
    EventLoop loop;
    loop.loop();
}
 
int main() {
    printf("main(): pid = %d, tid = %ld\n", getpid(), pthread_self());
    
    EventLoop loop;

    Thread thread(threadFunc);
    thread.start();
 
    loop.loop();
    pthread_exit(NULL);

    return 0;
}
