/*********************************************
* file:   TinySearchEngine.cc
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-07-07 17:52:23
**********************************************/
#include "../include/InetAddress.h"
#include "../include/TcpConnection.h"
#include "../include/EventLoop.h"
#include "../include/TcpServer.h" 
#include "../include/ThreadPool.h"
#include "../include/WordQuery.h"
#include "../include/Configure.h"
#include "../include/Callbacks.h"
#include <iostream>
using namespace tinyse;
using namespace::placeholders;

class TinySearchEngine : Uncopyable {
public:
    TinySearchEngine(const string &confiureFile, EventLoop &loop, InetAddress &listenAddr);
    void start();

private:
    void onConnection(const TcpConnectionPtr &conn);
    void onMessage(const TcpConnectionPtr &conn, Buffer *buf);
    void doTask(const TcpConnectionPtr &conn, const string &msg);
    
private:
    TcpServer m_server;
    ThreadPool m_pool;
    Configure m_conf;
    WordQuery m_wordQuery;
};
    
TinySearchEngine::TinySearchEngine(const string &confiureFile, EventLoop &loop, InetAddress &listenAddr)
    : m_server(&loop, listenAddr, "tinyse_server")
    , m_pool("worker")
    , m_conf(confiureFile)
    , m_wordQuery(m_conf) {

    LogInfo("new tinyse_server");
    m_server.setConnectionCallback(bind(&TinySearchEngine::onConnection, this, _1));
    m_server.setMessageCallback(bind(&TinySearchEngine::onMessage, this, _1, _2));
}

/* 启动服务器 */
void TinySearchEngine::start() {
    m_pool.start(8);
    m_server.start();
}

void TinySearchEngine::onConnection(const TcpConnectionPtr &conn) {
    //do nothing
    (void)conn; //防止warn
}

void TinySearchEngine::onMessage(const TcpConnectionPtr &conn, Buffer *buf) {
    string msg(buf->retrieveAsString());
    size_t pos = msg.find('\n');
    msg = msg.substr(0, pos);
    cout << msg << endl;
    m_pool.addTask(bind(&TinySearchEngine::doTask, this, conn, msg));
}

void TinySearchEngine::doTask(const TcpConnectionPtr &conn, const string &msg) {
    string ret = m_wordQuery.query(msg);
    
    int sz = ret.size();
    string message(to_string(sz));
    message.append("\n").append(ret);

    conn->send(message);
}

int main() {
    InetAddress listenAddr(8888);
    EventLoop loop;
    TinySearchEngine tinyse_server("../conf/online.conf", loop, listenAddr);

    tinyse_server.start();
    loop.loop();
 
    return 0;
}
