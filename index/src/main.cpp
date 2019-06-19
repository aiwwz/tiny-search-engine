/*********************************************
 * file:   main.cpp
 * author: AIWWZ(wzj1524@qq.com)
 * date:   2019-06-18 20:25:13
 **********************************************/
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
using namespace std;

int main() {

    cout << ::getcwd(NULL, 0) << endl;
    string oldPwd = ::getcwd(NULL, 0); //保存当前路径
    ::chdir("../include");
    cout << ::getcwd(NULL, 0) << endl;
    string pwd = ::getcwd(NULL, 0); //获取语料库目录的绝对路径
    ::chdir(oldPwd.c_str()); //还原当前路径
    cout << ::getcwd(NULL, 0) << endl;

    cout << "pwd = " << pwd << endl;

    return 0;
}
