/*********************************************
 * file:   TcpConnection.cc
 * author: AIWWZ(wzj1524@qq.com)
 * date:   2019-07-03 21:11:46
 **********************************************/
#include "../include/TcpConnection.h"
#include "../include/Channel.h"
#include "../include/Socket.h"
#include "../include/Channel.h"
#include "../include/MyLogger.h"
#include <unistd.h>
#include <functional>
using namespace tinyse;
using std::bind;  using namespace std::placeholders;

const char* TcpConnection::stateToString() const {
    switch (m_state) {
    //case kDisconnected:
    //    return "kDisconnected";
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
          // m_channel_->setWriteCallback(bind(&TcpConnection::handleWrite, this));
          // m_channel_->setCloseCallback(bind(&TcpConnection::handleClose, this));
          // m_channel_->setErrorCallback(bind(&TcpConnection::handleError, this));
          LogDebug("[%s] at %d fd = %d", m_name, this, sockfd);
      }

TcpConnection::~TcpConnection() {
    LogDebug("[%s] at %d fd = %d state = %s", m_name, this, m_channel->fd(), stateToString());
}


void TcpConnection::handleRead() {
    char buf[65536] = {0};
    ssize_t n = ::read(m_channel->fd(), buf, sizeof(buf));
    m_messageCallback(shared_from_this(), buf, n);
}

void TcpConnection::connectEstablished() {
}
