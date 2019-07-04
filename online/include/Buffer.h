/*********************************************
 * file:   Buffer.h
 * author: AIWWZ(wzj1524@qq.com)
 * date:   2019-07-04 10:53:05
 **********************************************/
#ifndef __BUFFER_H__
#define __BUFFER_H__
#include "Callbacks.h"
#include <vector>
#include <cassert>
using std::vector;

namespace tinyse {

class Buffer {
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    Buffer()
        : m_buffer(kCheapPrepend + kInitialSize)
        , m_readerIndex(kCheapPrepend)
        , m_writerIndex(kCheapPrepend) {

        assert(readableBytes() == 0);
        assert(writableBytes() == kInitialSize);
        assert(prependableBytes() == kCheapPrepend);
    }

    // default copy-ctor, dtor and assignment are fine

    void swap(Buffer& rhs) {
        m_buffer.swap(rhs.m_buffer);
        std::swap(m_readerIndex, rhs.m_readerIndex);
        std::swap(m_writerIndex, rhs.m_writerIndex);
    }

    size_t readableBytes() const { 
        return m_writerIndex - m_readerIndex;  
    }

    size_t writableBytes() const { 
        return m_buffer.size() - m_writerIndex;  
    }

    size_t prependableBytes() const { 
        return m_readerIndex;  
    }

    const char* peek() const { 
        return begin() + m_readerIndex;  
    }

    void retrieve(size_t len) {
        assert(len <= readableBytes());
        m_readerIndex += len;
    }

    void retrieveUntil(const char *end) {
        assert(peek() <= end);
        assert(end <= beginWrite());
        retrieve(end - peek());
    }

    void retrieveAll() {
        m_readerIndex = kCheapPrepend;
        m_writerIndex = kCheapPrepend;
    }

    std::string retrieveAsString() {
        std::string str(peek(), readableBytes());
        retrieveAll();
        return str;
    }

    void append(const std::string &str) {
        append(str.data(), str.length());
    }

    void append(const char *data, size_t len) {
        ensureWritableBytes(len);
        std::copy(data, data+len, beginWrite());
        hasWritten(len);
    }

    void append(const void *data, size_t len) {
        append(static_cast<const char*>(data), len);
    }

    void ensureWritableBytes(size_t len) {
        if (writableBytes() < len) {
            makeSpace(len);
        }
        assert(writableBytes() >= len);
    }

    char* beginWrite() { 
        return begin() + m_writerIndex;  
    }

    const char* beginWrite() const { 
        return begin() + m_writerIndex; 
    }

    void hasWritten(size_t len) {
        m_writerIndex += len;  
    }

    void prepend(const void *data, size_t len) {
        assert(len <= prependableBytes());
        m_readerIndex -= len;
        const char* d = static_cast<const char*>(data);
        std::copy(d, d+len, begin()+m_readerIndex);
    }

    void shrink(size_t reserve) {
        std::vector<char> buf(kCheapPrepend+readableBytes()+reserve);
        std::copy(peek(), peek()+readableBytes(), buf.begin()+kCheapPrepend);
        buf.swap(m_buffer);
    }

    ssize_t readFd(int fd);

private:

    char* begin() { 
        return &*m_buffer.begin();  
    }

    const char* begin() const {
        return &*m_buffer.begin();
    }

    void makeSpace(size_t len) {
        if (writableBytes() + prependableBytes() < len + kCheapPrepend) {
            m_buffer.resize(m_writerIndex+len);
        }
        else {
            assert(kCheapPrepend < m_readerIndex);
            size_t readable = readableBytes();
            std::copy(begin()+m_readerIndex, begin()+m_writerIndex, begin()+kCheapPrepend);
            m_readerIndex = kCheapPrepend;
            m_writerIndex = m_readerIndex + readable;
            assert(readable == readableBytes());
        }
    }

private:
    vector<char> m_buffer;
    size_t m_readerIndex;
    size_t m_writerIndex;
};

} //end of namespace tinyse

#endif /* __BUFFER_H__ */
