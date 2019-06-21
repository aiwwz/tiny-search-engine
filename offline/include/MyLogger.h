#ifndef __MYLOGGER_H__
#define __MYLOGGER_H__
#include <pthread.h>
#include <cstdlib>
#include <log4cpp/Category.hh>
#include <string>

#define prefix(msg) (std::string("[")+__FILE__+":"+__FUNCTION__+":"+std::to_string(__LINE__)+"] "+msg).c_str()
#define LogError(msg, ...) MyLogger::getInstance()->error(prefix(msg), ##__VA_ARGS__)
#define LogWarn(msg, ...)  MyLogger::getInstance()->warn(prefix(msg), ##__VA_ARGS__)
#define LogInfo(msg, ...)  MyLogger::getInstance()->info(prefix(msg), ##__VA_ARGS__)
#define LogDebug(msg, ...) MyLogger::getInstance()->debug(prefix(msg), ##__VA_ARGS__)

class MyLogger {
public:
    static MyLogger* getInstance() {
        if(nullptr == m_pInstance) {
            pthread_once(&m_once_control, init); //确保线程安全
        }
        return m_pInstance;
    }

    void error(const char *msg);

    template <typename ... Args>
    void error(Args ... args) { m_logger.error(args...); }

    void warn(const char *msg);
    
    template <typename ... Args>
    void warn(Args ... args) { m_logger.warn(args...); }

    void info(const char *msg);

    template <typename ... Args>
    void info(Args ... args) { m_logger.info(args...); }

    void debug(const char *msg);
    
    template <typename ... Args>
    void debug(Args ... args) { m_logger.debug(args...); }

private:
    MyLogger();
    ~MyLogger();

    static void destroy() {
        if(nullptr != m_pInstance) {
            delete m_pInstance;
            m_pInstance = nullptr;
        }
    }

    static void init() {
        m_pInstance = new MyLogger();
        atexit(destroy);
    }

private:
    static MyLogger *m_pInstance;
    static pthread_once_t m_once_control;
    log4cpp::Category & m_logger;
};


#endif /* __MYLOGGER_H__ */
