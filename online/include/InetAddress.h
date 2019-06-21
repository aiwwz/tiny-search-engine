/*********************************************
* file:   InetAddress.h
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-13 10:10:37
**********************************************/
#ifndef __INETADDRESS_H__
#define __INETADDRESS_H__
#include <sys/socket.h> 
#include <sys/types.h>

namespace tinyse {

/* 封装IP地址 */
class InetAddress {
public:
            

private:
    struct sockaddr_in m_addr;
};

} //end of namespace tinyse
 
#endif /* __INETADDRESS_H__ */
