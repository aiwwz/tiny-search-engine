/*********************************************
* file:   test8.cc
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-07-04 08:19:02
**********************************************/
#include <iostream>
#include "../include/TcpConnection.h"
#include "../include/TcpServer.h"
#include "../include/EventLoop.h"
#include "../include/Buffer.h"
#include <cstdio>
#include <unistd.h>
using namespace std;
using namespace tinyse;

void onConnection(const TcpConnectionPtr &conn) {
    if(conn->connected()) {
        printf("onConnection(): new connection [%s] from %s:%d\n", conn->name().c_str(), conn->peerAddr().ip().c_str(), conn->peerAddr().port());
    }
    else {
        printf("onConnection(): connection [%s] is down\n", conn->name().c_str());
    }
}

void onMessage(const TcpConnectionPtr &conn, Buffer *data) {
      printf("onMessage(): received %lu bytes from connection [%s]\n",  data->readableBytes(), conn->name().c_str());
      conn->send(data->retrieveAsString());
}

 
int main() {
    printf("main(): pid = %d\n", getpid());
    InetAddress listenAddr(8888);
    EventLoop loop;

    TcpServer server(&loop, listenAddr, "server");
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);
    server.start();

    loop.loop();
 
    return 0;
}
