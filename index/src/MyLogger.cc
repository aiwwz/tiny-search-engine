#include <iostream>
#include "../include/MyLogger.h"
#include <log4cpp/PropertyConfigurator.hh>
using namespace log4cpp;

/* 初始化静态成员 */
MyLogger *MyLogger::m_pInstance = nullptr;
pthread_once_t MyLogger::m_once_control = PTHREAD_ONCE_INIT;

MyLogger::MyLogger() : m_logger(Category::getRoot().getInstance("logger")) {
    try {
        PropertyConfigurator::configure("../conf/log4cpp.conf");
    }
    catch(ConfigureFailure &err) {
        std::cerr << "Configure failure: " << err.what() << std::endl;
        exit(1);
    }
}

MyLogger::~MyLogger() {
    Category::shutdown();
}

void MyLogger::error(const char *msg) {
    m_logger.error(msg);
}

void MyLogger::warn(const char *msg) {
    m_logger.warn(msg);
}

void MyLogger::info(const char *msg) {
    m_logger.info(msg);
}

void MyLogger::debug(const char *msg) {
    m_logger.debug(msg);
}

