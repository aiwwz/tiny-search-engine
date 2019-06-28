/*********************************************
* file:   Acceptor.h
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-13 11:17:51
**********************************************/
#ifndef __ACCEPTOR_H__
#define __ACCEPTOR_H__
#include <functional>
#include "Socket.h"
#include "Channel.h"
#include "Uncopyable.h"

namespace tinyse {

class InetAddress;
class EventLoop;

/* 封装accept, 用于服务端接受连接, 并通过回调通知使用者 */
class Acceptor : Uncopyable {
    using NewConnectCallback = std::function<void(int, const InetAddress&)>;
public:
    Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reuseport = true);

    void setNewConnectCallback(const NewConnectCallback &cb) {
        m_newConnectCallback = cb;
    }

    bool listenning() const {
        return m_listenning;
    }

    void listen();

private:
    void handleRead();

private:
    bool m_listenning;
    EventLoop *m_loop;
    Socket m_socket;   //listen套接字
    Channel m_channel; //用于观察此socket上的可读事件
    NewConnectCallback m_newConnectCallback;
};

} //end of namespace tinyse
 
#endif /* __ACCEPTOR_H__ */
