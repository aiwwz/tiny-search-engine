/*********************************************
* file:   DirScanner.cc
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-18 22:32:12
**********************************************/
#include "../include/DirScanner.h"
#include "../include/MyLogger.h"
#include "../include/Configure.h"
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <cstring>
#include <iostream> //for debug
using namespace tinyse;
using std::cout;  using std::endl;

DirScanner::DirScanner(Configure &config) : m_config(config) {
    traverse(m_config.getConfigMap()["corpus"]); //获取语料库中所有语料的绝对路径
}


/* 遍历语料库目录下的所有文件, 获取其绝对路径 */
inline void DirScanner::traverse(const string &corpusDir) {
    DIR *dir = ::opendir(corpusDir.c_str());
    if(nullptr == dir) { //语料库目录打开失败: 直接终止程序
        LogError("Can't open dir %s", corpusDir.c_str());
        exit(-1);
    }

    string oldPwd = ::getcwd(NULL, 0); //保存当前路径
    ::chdir(corpusDir.c_str());
    string pwd = ::getcwd(NULL, 0); //获取语料库目录的绝对路径

    struct dirent *entry; //目录下的一项(可能是普通文件, 也可能是目录)
    struct stat st;
    while((entry = ::readdir(dir))) {
        ::stat(entry->d_name, &st); //获取该项的信息

        if(S_ISDIR(st.st_mode)) { //是目录
            if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue; //任何目录下都有此两项, 忽略之
            }
            else { //普通目录, 则递归获取其中的语料信息
                traverse(entry->d_name); 
            }
        }
        else { //非目录, 则记录其绝对路径
            m_vecCorpus.push_back(pwd + "/" + entry->d_name);
        }
    }

    ::chdir(oldPwd.c_str()); //还原当前路径
    ::closedir(dir);
}

inline const vector<string>& DirScanner::getCorpusFiles() const {
    return m_vecCorpus;
}

void DirScanner::print() const {
    cout << endl << "Print corpus:" << endl;
    for(auto &it : m_vecCorpus) {
        cout << it << endl;
    }
}
