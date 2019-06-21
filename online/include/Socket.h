/*********************************************
* file:   Socket.h
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-13 11:14:32
**********************************************/
#ifndef __SOCKET_T__
#define __SOCKET_T__

namespace tinyse {

 /* 封装socket描述符, 负责创建/关闭套接字 */
class Socket {
public:
    Socket(); 
    Socket(int fd);
    ~Socket();
    
    void shutdownWrite();
private:
    int m_fd;
};
 
} //end of namespace tinyse
 
#endif /* __SOCKET_T__ */
