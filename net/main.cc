/*********************************************
 * file:   main.cc
 * author: AIWWZ(wzj1524@qq.com)
 * date:   2019-06-15 21:15:01
 **********************************************/
#include <iostream>
#include <string>
using namespace std;

string toString() {
    char buf[32] = {0};
    int64_t seconds = 1134124215463456 / 1000000;
    int64_t microseconds = 1134124215463456 % 1000000;
    snprintf(buf, sizeof(buf)-1, "%ld.%06ld", seconds, microseconds);
    return buf;
}

int main() {
    cout << toString() << endl;
    return 0;
}
