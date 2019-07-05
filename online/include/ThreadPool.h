/*********************************************
* file:   ThreadPool.h
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-07-05 15:43:05
**********************************************/
#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__
#include <vector>
#include <string>
#include <queue>
#include <functional>
#include "MutexLock.h"
#include "Condition.h"
#include "Uncopyable.h"
using std::vector;  using std::queue;  using std::string;
using std::function;
 
namespace tinyse {

class Thread;

class ThreadPool : Uncopyable {
public:
    using Task = function<void()>;

    ThreadPool(const string &name); 
    ~ThreadPool();

    void start(int threadNums);
    void stop();
    void addTask(const Task &task);

private:
    void threadFunc();
    Task getTask();

private:
    vector<Thread*> m_threads; //存放线程的容器
    queue<Task> m_taskQue;     //任务队列
    MutexLock m_mutex;
    Condition m_cond;
    bool m_running = false;
    string m_name;
};

} //end of namespace tinyse
 
#endif /* __THREADPOOL_H__ */
