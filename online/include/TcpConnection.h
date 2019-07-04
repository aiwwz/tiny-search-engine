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
#include "Buffer.h"
using std::unique_ptr;
 
namespace tinyse {

class EventLoop;
class Socket;
class Channel;

/*  */
class TcpConnection : Uncopyable
                    , public std::enable_shared_from_this<TcpConnection> {
    using State = enum { kConnecting, kConnected, kDisconnecting, kDisconnected};
public:
    TcpConnection(EventLoop *loop, const string &name, int sockfd, const InetAddress &localAddr, const InetAddress &peerAddr);
    ~TcpConnection();

    void setConnectionCallback(const ConnectionCallback &cb) {
        m_connectionCallback = cb;
    }

    void setMessageCallback(const MessageCallback &cb) {
        m_messageCallback = cb;
    }

    void setCloseCallback(const CloseCallback &cb) {
        m_closeCallback = cb;
    }

    void connectEstablished();
    void connectDestroyed();  //当TcpServer将本对象移出map时调用

    bool connected() const {
        return m_state == kConnected;
    }

    const string& name() const {
        return m_name;
    }

    const InetAddress& localAddr() const {
        return m_localAddr;
    }

    const InetAddress& peerAddr() const {
        return m_peerAddr;
    }

    void send(const string &message);
    void shutdown();

private:
    void setState(State st) { 
        m_state = st; 
    }
    const char* stateToString() const;

    void handleRead();
    void handleWrite();
    void handleClose();
    void sendInLoop(const string &message);
    void shutdownInLoop();

private:
    EventLoop *m_loop;
    string m_name;
    State m_state = kConnecting;
    unique_ptr<Socket> m_socket;
    unique_ptr<Channel> m_channel;
    InetAddress m_localAddr;
    InetAddress m_peerAddr;
    ConnectionCallback m_connectionCallback;
    MessageCallback m_messageCallback;
    CloseCallback m_closeCallback; //此回调绑定到TcpServer::removeConnection()
    Buffer m_inputBuffer;
    Buffer m_outputBuffer;
};

} //end of namespace tinyse
 
#endif /* __TCPCONNECTION_H__ */
