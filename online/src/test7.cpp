/*********************************************
* file:   test7.cc
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-07-02 15:10:09
**********************************************/
#include <iostream>
#include <cstdio>
#include "../include/Acceptor.h"
#include "../include/EventLoop.h"
#include "../include/InetAddress.h"
using namespace std;
using namespace tinyse;

void newConnection(int sockfd, const InetAddress &peerAddr) {
    printf("newConnection(): accepted a new connection from %s\n", peerAddr.ip().c_str());
    ::write(sockfd, "How are you?\n", 13);
    ::close(sockfd);
}

void newConnection2(int sockfd, const InetAddress &peerAddr) {
    printf("newConnection(): accepted a new connection from %s\n", peerAddr.ip().c_str());
    ::write(sockfd, "I am fine!\n", 11);
    ::close(sockfd);
}

int main() {
    printf("main(): pid = %d\n", getpid());

    EventLoop loop;

    InetAddress listenAddr(8888);
    Acceptor acceptor(&loop, listenAddr);
    acceptor.setNewConnectionCallback(newConnection);
    acceptor.listen();

    InetAddress listenAddr2(8889);
    Acceptor acceptor2(&loop, listenAddr2);
    acceptor2.setNewConnectionCallback(newConnection2);
    acceptor2.listen();

    loop.loop();
 
    return 0;
}
