#ifndef __TINYLOGGER_H__
#define __TINYLOGGER_H__
#include <pthread.h>
#include <cstdlib>
#include <log4cpp/Category.hh>
#include <string>

#define prefix(msg) (std::string("[")+__FILE__+":"+__FUNCTION__+":"+std::to_string(__LINE__)+"] "+msg).c_str()
#define logError(msg, ...) tinyLogger::getInstance()->error(prefix(msg), ##__VA_ARGS__)
#define logWarn(msg, ...)  tinyLogger::getInstance()->warn(prefix(msg), ##__VA_ARGS__)
#define logInfo(msg, ...)  tinyLogger::getInstance()->info(prefix(msg), ##__VA_ARGS__)
#define logDebug(msg, ...) tinyLogger::getInstance()->debug(prefix(msg), ##__VA_ARGS__)

class tinyLogger {
public:
    static tinyLogger* getInstance() {
        if(nullptr == m_pInstance) {
            pthread_once(&m_once_control, init); //确保线程安全
        }
        return m_pInstance;
    }

    static void destroy() {
        if(nullptr != m_pInstance) {
            delete m_pInstance;
            m_pInstance = nullptr;
        }
    }

    static void init() {
        m_pInstance = new tinyLogger();
        atexit(destroy);
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
    tinyLogger();
    ~tinyLogger();

    static tinyLogger *m_pInstance;
    static pthread_once_t m_once_control;
    log4cpp::Category & m_logger;
};


#endif /* __TINYLOGGER_H__ */
