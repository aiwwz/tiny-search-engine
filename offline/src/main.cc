/*********************************************
* file:   main.cc
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-18 19:00:31
**********************************************/
#include <iostream>
#include "../include/Configure.h"
#include "../include/DirScanner.h"
#include "../include/PageLib.h"
using namespace std;
 
int main() {
    tinyse::Configure config("../conf/offline.conf"); //加载配置文件

    tinyse::PageLib pageLib(config); //存储格式化后的网页和偏移库
    pageLib.doit();
 
    return 0;
}
