/*********************************************
* file:   test.cc
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-09 12:36:36
**********************************************/
#include <iostream>
#include "../include/MutexLock.h"
#include "../include/Condition.h"
using namespace std;
using namespace tinyse;


MutexLock mutex;
Condition cond(mutex);
bool flag = false;
int g_int = 1;

void* threadFunc(void *) {
    cout << "start" << endl;
    while(false == flag) {
        MutexLockGuard lock(mutex);
        cond.wait();
    }    
    cout << g_int++ << endl;

    return nullptr;
}

int main() {
    pthread_t tid1;
    pthread_t tid2;
    pthread_create(&tid1, nullptr, threadFunc, nullptr);
    pthread_create(&tid2, nullptr, threadFunc, nullptr);

    {
        MutexLockGuard lock(mutex);
        flag = true;
        cond.signal();
    }
    cout << "--------------" << endl;
    {
        MutexLockGuard lock(mutex);
        flag = true;
        cond.signal();
    }

    pthread_join(tid1, nullptr);
    pthread_join(tid2, nullptr);

    return 0;
}
