/*********************************************
* file:   CurrentThread.h
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-14 16:57:00
**********************************************/
#ifndef __CURRENTTHREAD_H__ 
#define __CURRENTTHREAD_H__ 
#include "../include/Config.h"
#include <unistd.h>
#include <syscall.h>
#include <sys/types.h>

BEGIN_NAMESPACE_TINYSE

pid_t gettid() { //待优化: 每次获取tid都要系统调用
    return syscall(SYS_gettid);
}

END_NAMESPACE_TINYSE
 
#endif /* __CURRENTTHREAD_H__  */
