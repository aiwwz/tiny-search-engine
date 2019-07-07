/*********************************************
* file:   test11.cc
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-07-05 21:39:42
**********************************************/
#include <iostream>
#include <unistd.h>
#include "../include/ThreadPool.h"
using namespace std;
using namespace tinyse;

void threadFunc() {
    cout << "hello, world!" << endl;
}
 
int main() {
    ThreadPool pool("worker");
    pool.start(10);
    pool.addTask(threadFunc);

    return 0;
}
