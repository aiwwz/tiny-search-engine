/*********************************************
* file:   Channel.h
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-14 15:36:47
**********************************************/
#ifndef __CHANNEL_H__
#define __CHANNEL_H__
#include "../include/Config.h"
#include "Uncopyable.h"
#include <functional>

BEGIN_NAMESPACE_NET 

class EventLoop;

/* Channel负责将一个文件描述符的I/O事件的分发为不同的回调 */
class Channel : Uncopyable {
    using EventCallback = std::function<void()>;
public:
    Channel(EventLoop *loop, int fd)
        : m_loop(loop)
        , m_fd(fd)    
        , m_events(0) 
        , m_revents(0)
        , m_index(-1) { }

    void setReadCallback(const EventCallback &cb) {
        m_readCallback = cb;
    }

    void setWriteCallback(const EventCallback &cb) {
        m_writeCallback = cb;
    }

    void setErrorCallback(const EventCallback &cb) {
        m_errorCallback = cb;
    }

    void handleEvent();
    void enableReading() {
        m_events |= kReadEvent;
        update();
    }
    //void enableWriting();
    //void enableAll();
    
    int fd() const {
        return m_fd;
    }
    
    int events() const {
        return m_events;
    }

    bool isNoneEvent() const {
        return m_events == kNoneEvent;
    }

    void setRevents(const int revent) {
        m_revents = revent;
    }

    //在Poller中使用
    int index() const {
        return m_index;
    }

    void setIndex(const int idx) {
        m_index = idx;
    }

    EventLoop* ownerLoop() const {
        return m_loop;
    }

private:
    void update();

private:
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventCallback m_readCallback;
    EventCallback m_writeCallback;
    EventCallback m_errorCallback;

    EventLoop  *m_loop;
    const int   m_fd;
    int         m_events;  //关注的I/O事件
    int         m_revents; //当前活动的事件
    int         m_index;   //
};

END_NAMESPACE_NET
 
#endif /* __CHANNEL_H__ */
