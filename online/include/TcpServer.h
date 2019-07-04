/*********************************************
* file:   TcpServer.h
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-07-03 17:11:13
**********************************************/
#ifndef __TCPSERVER_H__
#define __TCPSERVER_H__
#include "Uncopyable.h"
#include "Callbacks.h"
#include <map>
using std::map; 

namespace tinyse {

class EventLoop;
class Acceptor;
class InetAddress;

/* 管理accept获得的TcpConnection */
class TcpServer : Uncopyable {
    using ConnectionMap = std::map<string, TcpConnectionPtr>;
public:
    TcpServer(EventLoop *loop, const InetAddress &listenAddr, const string &name, bool reuseport = true);
    ~TcpServer();

    void start();

    void setConnectionCallback(const ConnectionCallback &cb) {
        m_connectionCallback = cb;
    }

    void setMessageCallback(const MessageCallback &cb) {
        m_messageCallback = cb;
    }

private:
    void newConnection(int sockfd, const InetAddress &peerAddr);

private:
    EventLoop *m_loop;
    const string m_name; //Tcp Server的名字
    std::unique_ptr<Acceptor> m_acceptor; //通过acceptor获取新连接的fd
    ConnectionCallback m_connectionCallback;
    MessageCallback m_messageCallback;
    bool m_started = false;
    int m_nextConnectionID = 1;  //
    ConnectionMap m_connections; //<TcpConnection的名字, TcpConnectionPtr>
};

} //end of namespace tinyse
 
#endif /* __TCPSERVER_H__ */
