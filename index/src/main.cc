/*********************************************
* file:   main.cc
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-18 19:00:31
**********************************************/
#include <iostream>
#include "../include/Configure.h"
#include "../include/DirScanner.h"
using namespace std;
 
int main() {
    tinyse::Configure config("../conf/index.conf");    
    config.print();

    tinyse::DirScanner dirScanner(config);
    dirScanner.print();
 
    return 0;
}
