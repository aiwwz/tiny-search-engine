#include <iostream>
#include "../include/tinyLogger.h"
#include <log4cpp/PropertyConfigurator.hh>
using namespace log4cpp;

tinyLogger *tinyLogger::m_pInstance = nullptr;
pthread_once_t tinyLogger::m_once_control = PTHREAD_ONCE_INIT;

tinyLogger::tinyLogger() : m_logger(Category::getRoot().getInstance("logger")) {
    try {
        PropertyConfigurator::configure("../conf/log4cpp.conf");
    }
    catch(ConfigureFailure &err) {
        std::cerr << "Configure failure: " << err.what() << std::endl;
        exit(1);
    }
}

tinyLogger::~tinyLogger() {
    Category::shutdown();
}

void tinyLogger::error(const char *msg) {
    m_logger.error(msg);
}

void tinyLogger::warn(const char *msg) {
    m_logger.warn(msg);
}

void tinyLogger::info(const char *msg) {
    m_logger.info(msg);
}

void tinyLogger::debug(const char *msg) {
    m_logger.debug(msg);
}

