/*********************************************
* file:   ThreadPool.cc
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-07-05 16:41:40
**********************************************/
#include <cassert>
#include "../include/ThreadPool.h"
#include "../include/Thread.h"
#include "../include/MyLogger.h"
using namespace tinyse;
using std::bind;

ThreadPool::ThreadPool(const string &name) 
    : m_mutex() 
    , m_cond(m_mutex) 
    , m_name(name) {

    LogInfo("create new thread pool: %s", name.c_str());
}

ThreadPool::~ThreadPool() {
    if(m_running) {
        stop();
    }
}

void ThreadPool::start(int threadNums) {
    assert(m_threads.empty());
    m_running = true;
    m_threads.reserve(threadNums); //若threadNums<0, reserve将抛出异常
    //为线程池创建线程
    for(int idx = 0; idx < threadNums; ++idx) {
        char id[32] = {0};
        snprintf(id, sizeof(id), "#%d", idx+1);
        Thread *thread = new Thread(bind(&ThreadPool::threadFunc, this), m_name + id);
        m_threads.push_back(thread);
        LogInfo("create new thread: %s", thread->name().c_str());
        thread->start();
    }
}

void ThreadPool::stop() {
    if(m_running) {
        m_running = false;
        m_cond.broadcast();
        //回收线程
        for(auto it : m_threads) {
            it->join();
            delete it;
        }
        m_threads.clear();
    }
    LogInfo("stop thread pool!");
}

void ThreadPool::addTask(const Task &task) {
    if(m_threads.empty()) { //线程池中无线程, 则直接在主线程中运行
        task();
    }
    else {
        MutexLockGuard lock(m_mutex);
        m_taskQue.push(task);
        m_cond.signal();
        LogDebug("addTask()");
    }
}

ThreadPool::Task ThreadPool::getTask() {
    MutexLockGuard lock(m_mutex);
    while(m_taskQue.empty() && m_running) {
        m_cond.wait();
    }
    LogInfo("~~~ I am wakeup!"); 

    Task task;
    if(!m_taskQue.empty()) {
        task = m_taskQue.front();
        m_taskQue.pop();
        LogDebug("getTask()");
    }
    return task;
}

void ThreadPool::threadFunc() {
    while(m_running) {
        Task task = getTask();
        if(task) {
            task();
        }
    }
}
