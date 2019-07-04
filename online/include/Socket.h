/*********************************************
* file:   Socket.h
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-13 11:14:32
**********************************************/
#ifndef __SOCKET_T__
#define __SOCKET_T__
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "InetAddress.h"

namespace tinyse {

 /* 封装socket描述符, 负责创建/关闭套接字 */
class Socket {
public:
    Socket(int fd) : m_fd(fd) { }

    ~Socket() {
        ::close(m_fd);
    }
    
    int fd() {
        return m_fd;
    }

    void bind(const InetAddress &addr) {
        if(::bind(m_fd, addr.getSockAddrPtr(), sizeof(addr)) < 0) {
            perror("Socket: bind");
            exit(-1);
        }
    }

    void listen() {
        if(::listen(m_fd, SOMAXCONN) < 0) {
            perror("Socket: listen");
            exit(-1);
        }
    }

    int accept(InetAddress &peerAddr) {
        struct sockaddr_in addr;
        bzero(&addr, sizeof(addr));
        socklen_t addrlen = sizeof(addr);
        int connfd = ::accept4(m_fd, static_cast<struct sockaddr*>((void*)(&addr)), &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC); //使connfd上发生的I/O操作非阻塞
        if(connfd == -1) {
            perror("Socket: accept");
            exit(-1);
        }
        peerAddr.setSockAddr(addr);
        return connfd;
    }

    void shutdownWrite() {
        if(shutdown(m_fd, SHUT_WR) < 0) {
            perror("Socket: shutdown");        
            exit(-1);
        }
    }

    void setReuseAddr(bool flag) {
        int reuse = flag ? 1 : 0;
        if(setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0) {
            perror("Socket: setsockopt");
            exit(-1);
        }
    }

    void setReusePort(bool flag) {
        int reuse = flag ? 1 : 0;
        if(setsockopt(m_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(int)) < 0) {
            perror("Socket: setsockopt");
            exit(-1);
        }
    }

private:
    int m_fd;
};
 
} //end of namespace tinyse
 
#endif /* __SOCKET_T__ */
