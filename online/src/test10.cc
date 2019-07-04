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
std::string message1;
std::string message2;

void onConnection(const TcpConnectionPtr& conn)
{
    if (conn->connected())
    {
        sleep(3);
        conn->send(message1);
        conn->send(message2);
        conn->shutdown();
    }
    else
    {
        printf("onConnection(): connection [%s] is down\n",
               conn->name().c_str());
    }
}

void onMessage(const TcpConnectionPtr& conn, Buffer* buf)
{
    printf("onMessage(): received %zd bytes from connection [%s]\n",
           buf->readableBytes(),
           conn->name().c_str());

    buf->retrieveAll();
}

int main(int argc, char* argv[])
{
    printf("main(): pid = %d\n", getpid());

    int len1 = 500;
    int len2 = 300;

    if (argc > 2)
    {
        len1 = atoi(argv[1]);
        len2 = atoi(argv[2]);
    }

    message1.resize(len1);
    message2.resize(len2);
    std::fill(message1.begin(), message1.end(), 'A');
    std::fill(message2.begin(), message2.end(), 'B');

    InetAddress listenAddr(8888);
    EventLoop loop;

    TcpServer server(&loop, listenAddr, "server");
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);
    server.start();

    loop.loop();
}

