/*********************************************
* file:   MutexLock.h
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-09 11:46:31
**********************************************/
#ifndef __MUTEXLOCK_H__
#define __MUTEXLOCK_H__
#include "Uncopyable.h"
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <iostream>
using namespace std;

namespace tinyse {

class MutexLock : tinyse::Uncopyable {
    friend class MutexLockGuard;
public:
    MutexLock() : m_holder_pid(0) {
        pthread_mutex_init(&m_mutex, nullptr);
    }

    ~MutexLock() {
        assert(0 == m_holder_pid);
        pthread_mutex_destroy(&m_mutex);
    }

    pthread_mutex_t *getMutex() {
        return &m_mutex;
    } 

protected:
    /* 只能由MutexLockGuard调用, 防止用户代码调用 */
    void lock() {
        pthread_mutex_lock(&m_mutex);
        m_holder_pid = gettid();
    }

    void unlock() {
        m_holder_pid = 0;
        pthread_mutex_unlock(&m_mutex);
    }

    pid_t gettid() {
        return syscall(SYS_gettid);
    }

private:
    pthread_mutex_t m_mutex;
    pid_t m_holder_pid; //当前持有lock的线程id
};

class MutexLockGuard : tinyse::Uncopyable {
public:
    explicit MutexLockGuard(MutexLock &mutex) : m_mutex(mutex) {
        m_mutex.lock();
    }

    ~MutexLockGuard() {
        m_mutex.unlock();
    }

private:
    MutexLock &m_mutex;
};

} //end of namespace tinyse
 
#endif /* __MUTEXLOCK_H__ */
