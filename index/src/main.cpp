/*********************************************
 * file:   main.cpp
 * author: AIWWZ(wzj1524@qq.com)
 * date:   2019-06-18 20:25:13
 **********************************************/
#include "../include/PageLib.h"

int main() {
    tinyse::PageLib pageLib("../conf/pageLib.conf");
    pageLib.doit();

    return 0;
}
