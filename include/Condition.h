/*********************************************
* file:   Condition.h
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-09 13:01:56
**********************************************/
#ifndef __CONDITION_H__
#define __CONDITION_H__
#include <pthread.h>
#include "Uncopyable.h"
#include "MutexLock.h"
#include <cstdio>

namespace tinyse {

class Condition : tinyse::Uncopyable {
public:
    explicit Condition(MutexLock &mutex) : m_mutex(mutex) {
        if(pthread_cond_init(&m_cond, nullptr)) {
            perror("Condition.h: pthread_cond_init");
        }
    }

    ~Condition() {
        if(pthread_cond_destroy(&m_cond)) {
            perror("Condition.h: pthread_cond_destroy");
        }
    }

    void signal() {
        if(pthread_cond_signal(&m_cond)) {
            perror("Condition.h: pthread_cond_signal");
        }
    }

    void broadcast() {
        if(pthread_cond_broadcast(&m_cond)) {
            perror("Condition.h: pthread_cond_broadcast");
        }
    }

    void wait() {
        if(pthread_cond_wait(&m_cond, m_mutex.getMutex())) {
            perror("Condition.h: pthread_cond_wait");
        }
    }

private:
    MutexLock &m_mutex;
    pthread_cond_t m_cond;
};

} //end of namespace tinyse
 
#endif /* __CONDITION_H__ */
