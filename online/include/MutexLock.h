/*********************************************
* file:   MutexLock.h
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-09 11:46:31
**********************************************/
#ifndef __MUTEXLOCK_H__
#define __MUTEXLOCK_H__
#include "Uncopyable.h"
#include <pthread.h>
#include <cstdio>

namespace tinyse {

/* 封装互斥锁mutex */
class MutexLock : Uncopyable {
    friend class MutexLockGuard;
public:
    MutexLock() {
        if(pthread_mutex_init(&m_mutex, nullptr)) {
            perror("MutexLock.h: pthread_mutex_init");
        }
    }

    ~MutexLock() {
        if(pthread_mutex_destroy(&m_mutex)) {
            perror("MutexLock.h: pthread_mutex_destroy");
        }
    }

    pthread_mutex_t *getMutex() {
        return &m_mutex;
    } 

protected:
    /* 只能由MutexLockGuard调用, 防止用户代码调用 */
    void lock() {
        if(pthread_mutex_lock(&m_mutex)) {
            perror("MutexLock.h: pthread_mutex_lock");
        }
    }

    void unlock() {
        if(pthread_mutex_unlock(&m_mutex)) {
            perror("MutexLock.h: pthread_mutex_unlock");
        }
    }

private:
    pthread_mutex_t m_mutex;
};

/* 通过对象生命周期自动加/解锁 */
class MutexLockGuard : Uncopyable {
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
