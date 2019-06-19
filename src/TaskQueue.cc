/*********************************************
* file:   TaskQueue.cc
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-10 20:45:12
**********************************************/
#include "../include/TaskQueue.h"
using namespace tinyse;

namespace tinyse {

void TaskQueue::push(int elem) {
    { //缩小临界区范围
        MutexLockGuard lock(m_mutex);
        while(full()) {
            m_notFull.wait();
        }
        m_que.push(elem);
    } //临界区结束
    m_notEmpty.signal();
}

int TaskQueue::pop() {
    int retElem;
    { //缩小临界区范围
        MutexLockGuard lock(m_mutex);
        while(empty()) {
            m_notEmpty.wait();
        }
        retElem = m_que.front();
        m_que.pop();
    } //临界区结束
    m_notFull.signal();
    return retElem;
}

bool TaskQueue::full() const {
    return m_que.size() == m_size;
}

bool TaskQueue::empty() const {
    return m_que.empty();
}

} //end of namespace tinyse
