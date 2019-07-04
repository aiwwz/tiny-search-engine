/*********************************************
* file:   TcpConnection.h
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-07-03 18:15:34
**********************************************/
#ifndef __TCPCONNECTION_H__
#define __TCPCONNECTION_H__
#include "Callbacks.h"
#include "Uncopyable.h"
#include "InetAddress.h"
using std::unique_ptr;
 
namespace tinyse {

class EventLoop;
class Socket;
class Channel;

/*  */
class TcpConnection : Uncopyable
                    , public std::enable_shared_from_this<TcpConnection> {
    using State = enum { kConnecting, kConnected, };
public:
    TcpConnection(EventLoop *loop, const string &name, int sockfd, const InetAddress &localAddr, const InetAddress &peerAddr);
    ~TcpConnection();

    void setConnectionCallback(const ConnectionCallback &cb) {
        m_connectionCallback = cb;
    }

    void setMessageCallback(const MessageCallback &cb) {
        m_messageCallback = cb;
    }

    void connectEstablished();

private:
    void setState(State st) { 
        m_state = st; 
    }
    const char* stateToString() const;
    void handleRead();

private:
    EventLoop *m_loop;
    string m_name;
    State m_state = kConnecting; //默认状态是已建立好连接
    unique_ptr<Socket> m_socket;
    unique_ptr<Channel> m_channel;
    InetAddress m_localAddr;
    InetAddress m_peerAddr;
    ConnectionCallback m_connectionCallback;
    MessageCallback m_messageCallback;
};

} //end of namespace tinyse
 
#endif /* __TCPCONNECTION_H__ */
