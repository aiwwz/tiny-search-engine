/*********************************************
* file:   SocketsOps.h
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-07-03 18:46:35
**********************************************/
#ifndef __SOCKETSOPS_H__
#define __SOCKETSOPS_H__
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>
#include "MyLogger.h"
 
namespace tinyse {
namespace sockets {

struct sockaddr_in getLocalAddr(int sockfd) {
    struct sockaddr_in localaddr;
    bzero(&localaddr, sizeof(localaddr));
    socklen_t addrlen = static_cast<socklen_t>(sizeof(localaddr));

    if(::getsockname(sockfd, static_cast<struct sockaddr*>((void*)(&localaddr)), &addrlen) < 0) {
        perror("ScoketsOps::getLocalAddr");
    }

    return localaddr;
}

} //end of namespace sockets
} //end of namespace tinyse
 
#endif /* __SOCKETSOPS_H__ */
