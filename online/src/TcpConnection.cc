/*********************************************
 * file:   TcpConnection.cc
 * author: AIWWZ(wzj1524@qq.com)
 * date:   2019-07-03 21:11:46
 **********************************************/
#include "../include/TcpConnection.h"
#include "../include/EventLoop.h"
#include "../include/Channel.h"
#include "../include/Socket.h"
#include "../include/Channel.h"
#include "../include/MyLogger.h"
#include "../include/Buffer.h"
#include <unistd.h>
#include <functional>
using namespace tinyse;
using std::bind;  using namespace std::placeholders;

const char* TcpConnection::stateToString() const {
    switch (m_state) {
    case kDisconnected:
        return "kDisconnected";
    case kConnecting:
        return "kConnecting";
    case kConnected:
        return "kConnected";
    //case kDisconnecting:
    //    return "kDisconnecting";
    default:
        return "unknown state";
    }
}

TcpConnection::TcpConnection(EventLoop *loop, const string &name, int sockfd, const InetAddress &localAddr, const InetAddress &peerAddr)
    : m_loop(loop)
    , m_name(name)
    , m_socket(new Socket(sockfd))
    , m_channel(new Channel(loop, sockfd))
    , m_localAddr(localAddr)
    , m_peerAddr(peerAddr) { 

    m_channel->setReadCallback(bind(&TcpConnection::handleRead, this));
    m_channel->setWriteCallback(bind(&TcpConnection::handleWrite, this));
    m_channel->setCloseCallback(bind(&TcpConnection::handleClose, this));
    LogDebug("[%s] at %d fd = %d", m_name.c_str(), this, sockfd);
}

TcpConnection::~TcpConnection() {
    LogDebug("[%s] at %d fd = %d state = %s", m_name.c_str(), this, m_channel->fd(), stateToString());
}


void TcpConnection::handleRead() {
    ssize_t n = m_inputBuffer.readFd(m_channel->fd());

    if(n > 0) {
        m_messageCallback(shared_from_this(), &m_inputBuffer);
    } 
    else if(n == 0){
        handleClose();
    }
    else {
        //错误情况已在readFd中处理
    }
}

void TcpConnection::handleWrite() {
    m_loop->assertInLoopThread();
    if(m_channel->isWriting()) {
        ssize_t n = ::write(m_channel->fd(), m_outputBuffer.peek(), m_outputBuffer.readableBytes());
        if(n > 0) {
            m_outputBuffer.retrieve(n);
            if(m_outputBuffer.readableBytes() == 0) {
                m_channel->disableWriting();
                if(m_state == kDisconnecting) {
                    shutdownInLoop();
                }
            }
            else { //本次没写完
                LogInfo("I am going to write more data");
            }
        }
        else { //write出错
            LogError("write");
        }
    }
    else {
        LogInfo("Connection is down, no more writing");
    }
}

void TcpConnection::handleClose() {
    m_loop->assertInLoopThread();
    LogInfo("state = %s", stateToString());
    assert(m_state == kConnected || m_state == kDisconnecting);
    m_channel->disableAll();
    m_closeCallback(shared_from_this());
}

void TcpConnection::connectEstablished() {
    m_loop->assertInLoopThread();
    assert(m_state == kConnecting);
    setState(kConnected);
    m_channel->enableReading();
    m_connectionCallback(shared_from_this());
}

void TcpConnection::connectDestroyed() {
    m_loop->assertInLoopThread();
    assert(m_state == kConnected || m_state == kDisconnecting); 
    setState(kDisconnected);
    m_channel->disableAll();
    m_connectionCallback(shared_from_this());
    m_loop->removeChannel(m_channel.get());
}

void TcpConnection::send(const string &message) {
    if(m_state == kConnected) {
        if(m_loop->isInLoopThread()) {
            sendInLoop(message);
        }
        else {
            m_loop->runInLoop(bind(&TcpConnection::sendInLoop, this, message));
        }
    }
}

void TcpConnection::sendInLoop(const string &message) {
    m_loop->assertInLoopThread();
    ssize_t nwrote = 0;
    if(!m_channel->isWriting() && m_outputBuffer.readableBytes() == 0) {
        nwrote = ::write(m_channel->fd(), message.data(), message.size());
        if(nwrote >= 0) {
            if(static_cast<size_t>(nwrote) < message.size()) {
                LogInfo("I am going to write more data");
            }
        }
        else {
            nwrote = 0;
            if(errno != EWOULDBLOCK) {
               LogError("write error"); 
            }
        }
    }
    assert(nwrote >= 0);
    //一次没发送完, 则将数据存到buffer中, 当可再次发送时重新发送
    if(static_cast<size_t>(nwrote) < message.size()) {
        m_outputBuffer.append(message.data()+nwrote, message.size()-nwrote);
        if(!m_channel->isWriting()) {
            m_channel->enableWriting();
        }
    }
}

void TcpConnection::shutdown() {
    if(m_state == kConnected) {
        setState(kDisconnecting);
        m_loop->runInLoop(bind(&TcpConnection::shutdownInLoop, this));
    }
}

void TcpConnection::shutdownInLoop() {
    m_loop->assertInLoopThread();
    if(!m_channel->isWriting()) {
        m_socket->shutdownWrite();
    }
}
