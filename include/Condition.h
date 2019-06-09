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
 
namespace tinyse {

class Condition : tinyse::Uncopyable {
public:
    explicit Condition(MutexLock &mutex) : m_mutex(mutex) {
        pthread_cond_init(&m_cond, nullptr);
    }

    ~Condition() {
        pthread_cond_destroy(&m_cond);
    }

    void signal() {
        pthread_cond_signal(&m_cond);
    }

    void broadcast() {
        pthread_cond_broadcast(&m_cond);
    }

    void wait() {
        pthread_cond_wait(&m_cond, m_mutex.getMutex());
    }

private:
    MutexLock &m_mutex;
    pthread_cond_t m_cond;
};

} //end of namespace tinyse
 
#endif /* __CONDITION_H__ */
