/*********************************************
* file:   Thread.cc
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-10 20:20:26
**********************************************/
#include "../include/Thread.h"
//#include "../include/CurrentThread.h"
#include <cstdio>
#include <unistd.h>
#include <syscall.h>
using namespace tinyse;

namespace tinyse {

Thread::~Thread() {
    if(m_started) {
        if(pthread_detach(m_pthid)) {
            perror("Thread.cc: pthread_detach");
        }
        m_started = false;
    }
}

void Thread::start() {
    if(pthread_create(&m_pthid, nullptr, threadFunc, this)) {
        perror("Thread.cc: pthread_create");
        return;
    }
    m_started = true;
}

void Thread::join() {
    if(m_started) {
        if(pthread_join(m_pthid, nullptr)) {
            perror("Thread: pthread_join");
            return;
        }
        m_started = false;
    }
}

void* Thread::threadFunc(void *obj) {
    Thread *thd = static_cast<Thread*>(obj);
    thd->m_tid = pthread_self(); //设置当前线程id
    if(nullptr != thd) {
        thd->m_func();
    }
    return nullptr;
}

pid_t Thread::tid() const {
    return m_tid;
}

} //end of namespace tinyse
