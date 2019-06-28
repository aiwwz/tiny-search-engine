/*********************************************
* file:   InetAddress.h
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-13 10:10:37
**********************************************/
#ifndef __INETADDRESS_H__
#define __INETADDRESS_H__
#include <sys/socket.h> 
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <string>
#include <cstring>
using std::string;

namespace tinyse {

/* 对struct sockaddr_in的简单封装 */
class InetAddress {
public:
    InetAddress(uint16_t port = 0) {
        bzero(&m_addr, sizeof(m_addr));
        m_addr.sin_family = AF_INET;
        m_addr.sin_port = htons(port);
        m_addr.sin_addr.s_addr = INADDR_ANY; //使用本机IP
    }

    InetAddress(const char *ip, uint16_t port) {
        bzero(&m_addr, sizeof(m_addr));
        m_addr.sin_family = AF_INET;
        m_addr.sin_port = htons(port);
        m_addr.sin_addr.s_addr = inet_addr(ip);
    }

    InetAddress(const struct sockaddr_in &addr)
        : m_addr(addr) { }

    const struct sockaddr* getSockAddrPtr() const {
        return static_cast<const struct sockaddr*>((void*)(&m_addr));
    }
    void setSockAddr(const sockaddr_in &addr) {
        m_addr = addr;
    }

    string ip() const {
        return inet_ntoa(m_addr.sin_addr);
    }

    uint16_t port() const {
        return ntohs(m_addr.sin_port);
    }

private:
    struct sockaddr_in m_addr;
};

} //end of namespace tinyse
 
#endif /* __INETADDRESS_H__ */
