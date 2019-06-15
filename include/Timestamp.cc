/*********************************************
* file:   TimeStamp.cc
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-15 21:03:38
**********************************************/
#include "Timestamp.h"
#include <cstdio>
#include <sys/time.h>
using namespace tinyse;

Timestamp::Timestamp() : m_microSecondsSinceEpoch(0) { }

Timestamp::Timestamp(int64_t microSecondsSinceEpoch)
    : m_microSecondsSinceEpoch(microSecondsSinceEpoch) { }

std::string Timestamp::toString() const {
    char buf[32] = {0};
    int64_t seconds = m_microSecondsSinceEpoch / kMicroSecondsPerSecond;
    int64_t microseconds = m_microSecondsSinceEpoch % kMicroSecondsPerSecond;
    snprintf(buf, sizeof(buf)-1, "%ld.%06ld", seconds, microseconds);
    return buf;
}

Timestamp Timestamp::now() {
    struct timeval tv;
    gettimeofday(&tv, nullptr); //获取microSecondsSinceEpoch
    return Timestamp(tv.tv_sec * kMicroSecondsPerSecond + tv.tv_usec); 
}

