/*********************************************
* file:   TimeStamp.h
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-15 19:26:26
**********************************************/
#ifndef __TIMESTAMP_H__
#define __TIMESTAMP_H__
#include <stdint.h> //for int64_t
#include <string>
#include <iostream>
using std::cout;  using std::endl;
 
namespace tinyse {

class Timestamp {
public:
    Timestamp();
    Timestamp(int64_t microSecondsSinceEpoch);
    ~Timestamp() { }

    void swap(Timestamp &rhs) {
        std::swap(m_microSecondsSinceEpoch, rhs.m_microSecondsSinceEpoch);
    }

    bool valid() const {
        return m_microSecondsSinceEpoch > 0;
    }

    static Timestamp invalid() { //获取一个非法时间
        return Timestamp();
    }

    int64_t microSecondsSinceEpoch() const {
        return m_microSecondsSinceEpoch;
    }

    std::string toString() const;
    static Timestamp now(); //获取当前时间

    static const int kMicroSecondsPerSecond = 1000000;

private:
    int64_t m_microSecondsSinceEpoch;
};

inline bool operator<(const Timestamp &lhs, const Timestamp &rhs) {
    return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline bool operator==(const Timestamp &lhs, const Timestamp &rhs) {
    return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

inline double timeDifference(const Timestamp &rhs, const Timestamp &lhs) {
    int64_t diff = rhs.microSecondsSinceEpoch() - lhs.microSecondsSinceEpoch();
    if(diff < 0) {
        diff = -diff;
    }
    return static_cast<double>(diff) / Timestamp::kMicroSecondsPerSecond;
}

inline Timestamp addTime(const Timestamp &timestamp, double seconds) {
    return Timestamp(timestamp.microSecondsSinceEpoch() + static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond));
}

} //end of namespace tinyse
 
#endif /* __TIMESTAMP_H__ */
