/*********************************************
* file:   TaskQue.h
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-10 09:11:56
**********************************************/
#ifndef __TASKQUEUE_H_
#define __TASKQUEUE_H_
#include <queue>
#include "MutexLock.h"
#include "Condition.h"
 
namespace tinyse {

class TaskQueue {
public:
    TaskQueue(size_t size = 10)
        : m_size(size)
        , m_que()
        , m_mutex()
        , m_notFull(m_mutex)
        , m_notEmpty(m_mutex) {}

    void push(int elem);
    int pop();
    bool full() const;
    bool empty() const;

private:
    size_t m_size;          //队列最大容量
    std::queue<int> m_que;  //任务队列
    MutexLock m_mutex;
    Condition m_notFull;
    Condition m_notEmpty;
};

} //end of namespace tinyse
 
#endif /* __TASKQUEUE_H_ */
