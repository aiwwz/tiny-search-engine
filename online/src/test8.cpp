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
      printf("onMessage(): [%s]\n", data->retrieveAsString().c_str());
}

 
int main() {
    printf("main(): pid = %d\n", getpid());
    InetAddress listenAddr(8888);
    EventLoop loop;

    TcpServer server(&loop, listenAddr, "server");
    cout << "1111111111111" << endl;
    server.setConnectionCallback(onConnection);
    cout << "2222222222222" << endl;
    server.setMessageCallback(onMessage);
    cout << "3333333333333" << endl;
    server.start();
    cout << "4444444444444" << endl;

    loop.loop();
 
    return 0;
}
