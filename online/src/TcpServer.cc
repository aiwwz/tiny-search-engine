/*********************************************
* file:   TcpServer.cc
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-07-03 17:57:35
**********************************************/
#include "../include/TcpServer.h"
#include "../include/EventLoop.h"
#include "../include/Acceptor.h"
#include "../include/MyLogger.h"
#include "../include/SocketsOps.h"
#include "../include/TcpConnection.h"
using namespace tinyse;
using namespace std::placeholders;

TcpServer::TcpServer(EventLoop *loop, const InetAddress &listenAddr, const string &name, bool option)
    : m_loop(loop)
    , m_name(name)
    , m_acceptor(new Acceptor(loop, listenAddr, option)) {

    m_acceptor->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, _1, _2));
}

TcpServer::~TcpServer() {

}

void TcpServer::start() {
    if(!m_started) {
        m_started = true;
    }

    if(!m_acceptor->listenning()) {
        m_loop->runInLoop(bind(&Acceptor::listen, m_acceptor.get()));
    }
}

/* 新连接到达时, acceptor会回调此函数 */
void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr) {
    m_loop->assertInLoopThread();
    char buf[32];
    snprintf(buf, sizeof(buf), "#%d", m_nextConnectionID++);
    string connName = m_name + buf; //每个TcpConnection对象一个唯一的名字
    
    LogInfo("new connection [%s] from %s:%d", connName.c_str(), peerAddr.ip().c_str(), peerAddr.port());
    InetAddress localAddr(sockets::getLocalAddr(sockfd)); 
    TcpConnectionPtr conn(make_shared<TcpConnection>(m_loop, connName, sockfd, localAddr, peerAddr));
    m_connections[connName] = conn;
    conn->setConnectionCallback(m_connectionCallback);
    conn->setMessageCallback(m_messageCallback);
    conn->setCloseCallback(bind(&TcpServer::removeConnection, this, _1));
    conn->connectEstablished();
}

void TcpServer::removeConnection(const TcpConnectionPtr &conn) {
    m_loop->assertInLoopThread();
    LogInfo("[%s] - connection %s", m_name.c_str(), conn->name().c_str());
    size_t n = m_connections.erase(conn->name());
    assert(n == 1); (void)n;
    m_loop->queueInLoop(bind(&TcpConnection::connectDestroyed, conn));
} 
