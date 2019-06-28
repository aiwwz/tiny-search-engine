/*********************************************
* file:   testAcceptor.cc
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-27 15:45:38
**********************************************/
#include "../include/Acceptor.h"
#include "../include/InetAddress.h"
#include "../include/EventLoop.h"
#include <iostream>
#include <cstdio>
using namespace tinyse;
using namespace std;

void newConnection(int sockfd, const InetAddress &peerAddr) {
    printf("newConnection(): accepted a new conection from %s\n", peerAddr.ip().c_str());
    ::write(sockfd, "How are you?\n", 13);
    ::close(sockfd);
}


int main() {
    printf("main(): pid = %ld", pthread_self());

    InetAddress listenAddr(9999);
    EventLoop loop;

    Acceptor acceptor(&loop, listenAddr);
    acceptor.setNewConnectCallback(newConnection);
    acceptor.listen();

    loop.loop();
}
