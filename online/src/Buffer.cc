/*********************************************
* file:   Buffer.cc
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-07-04 12:24:58
**********************************************/
#include <sys/uio.h>
#include <cerrno>
#include "../include/Buffer.h"
using namespace tinyse;

ssize_t Buffer::readFd(int fd) {
    char extrabuf[65536];
    const size_t writable = writableBytes();

    struct iovec vec[2];
    vec[0].iov_base = begin() + m_writerIndex;
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof(extrabuf);

    const ssize_t n = readv(fd, vec, 2); //由于epoll采用水平触发, 所以若未读完也不会丢失数据
    if(n < 0) {
        perror("Buffer::readFd::readv");
    }
    else if(static_cast<size_t>(n) <= writable){ //buffer未装满
        m_writerIndex += n; 
    }
    else {
        m_writerIndex = m_buffer.size();
        append(extrabuf, n - writable);
    }
    return n;
}
