/*********************************************
* file:   Acceptor.cc
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-27 08:32:56
**********************************************/
#include "../include/Acceptor.h"
#include "../include/EventLoop.h"
#include "../include/InetAddress.h"
#include <poll.h>
using namespace tinyse;

int createNonblockSocket() { //创建非阻塞socket
    int sockfd =::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if(sockfd < 0) {
        perror("Acceptor: socket");
    }

    return sockfd;
}

Acceptor::Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reuseport)
    : m_listenning(false)
    , m_loop(loop)
    , m_socket(createNonblockSocket()) 
    , m_channel(loop, m_socket.fd()) {

    m_socket.setReuseAddr(true);
    m_socket.setReusePort(reuseport);
    m_socket.bind(listenAddr);
    m_channel.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

void Acceptor::listen() {
    m_loop->assertInLoopThread();
    m_listenning = true;
    m_socket.listen();
    m_channel.enableReading();
}

void Acceptor::handleRead() {
    m_loop->assertInLoopThread();
    InetAddress peerAddr;
    int connfd = m_socket.accept(peerAddr); 

    if(m_newConnectionCallback) {
        m_newConnectionCallback(connfd, peerAddr);
    }
    else {
        ::close(connfd);
    }
}

