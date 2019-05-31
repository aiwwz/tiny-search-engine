#ifndef __TINYLOGGER_H__
#define __TINYLOGGER_H__
#include <cstdlib>

class tinyLogger {
public:
    tinyLogger* Instance() {
        if(nullptr == m_logger) {
            m_logger = new tinyLogger();
            atexit(Destroy);
        }
        return m_logger;
    }

    static void Destroy() {
        if(nullptr != m_logger) {
            delete m_logger;
            m_logger = nullptr;
        }
    }

    void error(const char *msg);
    void warn(const char *msg);
    void info(const char *msg);
    void debug(const char *msg);

private:
    tinyLogger() {}
    ~tinyLogger() {}

    static tinyLogger *m_logger;
};

/* 初始化静态成员 */
tinyLogger *tinyLogger::m_logger = nullptr;

#endif /* __TINYLOGGER_H__ */
