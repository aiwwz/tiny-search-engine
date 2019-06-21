/*********************************************
* file:   Configure.cc
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-18 18:25:43
**********************************************/
#include "../include/Configure.h"
#include "../include/MyLogger.h"
#include <sstream>
#include <cstdio>
#include <iostream> //for debug
using namespace tinyse;
using std::istringstream;  using std::cout;  using std::endl;


Configure::Configure(const string &filepath) {
    defaultConfig();      //初始化默认配置

    ifstream configs(filepath);
    if(configs) {
        LogInfo("Load Configuration file: %s", filepath.c_str());
        loadConfig(configs); //加载配置
   }
    else { //打开配置文件失败
        LogWarn("Couldn't open configure file: %s", filepath.c_str());
        //使用默认配置
    }

    readStopWords();
}


/* 配置文件中没有指定路径时, 使用默认路径 */
void Configure::defaultConfig() {
    m_configMap["corpus"] = "../rss/"; //语料库
    m_configMap["pagelib"] = "../data/pagelib.dat"; //去重之后的网页库
    m_configMap["offsetlib"] = "../data/offsetlib.dat"; //去重之后的网页偏移库
    m_configMap["invertedindexlib"] = "../data/invertedindexlib.dat"; //倒排索引
    m_configMap["stopwords"] = "../data/stop_words.utf8"; //停用词
}


void Configure::loadConfig(ifstream &configs) {
    string line, item, path;
    while(getline(configs, line)) {
        istringstream record(line); 
        record >> item >> path;
        if(m_configMap.find(item) != m_configMap.end()) { //使用find可以防止向map中添加新项
            if(0 == path.size()) { //简单检查一下是否是空路径
                LogWarn("Empty Path of %s", item.c_str());
                //直接使用默认配置
            }
            else { //非空路径(但并不保证路径合法)
                m_configMap[item] = path;
            }
        }
        else { //非法配置项
            LogWarn("Invalid configuration item: %s", item.c_str());
            //忽略之
        }
        //防止污染下一次读取
        item.clear();
        path.clear();
    }
}

void Configure::readStopWords() {
    ifstream stopWords(getConfigMap()["stopwords"]);
    if(stopWords) {
        LogInfo("Read stop words");
        string word;
        while(stopWords >> word) {
            m_stopWords.insert(word);
        }
    }
    else {
        LogError("Couldn't open stopwords file");
        exit(-1);
    }
}


map<string, string> Configure::getConfigMap() {
    return m_configMap;
}

set<string>& Configure::getStopWords() {
    return m_stopWords;
}

void Configure::print() const { //for debug
    cout << endl << "Print config info:" << endl;
    for(auto &it : m_configMap) {
        cout << it.first << " -> " << it.second << endl;
    }
    cout << endl << "Print stop words:" << endl;
    for(auto &word : m_stopWords) {
        cout << word << "  ";
    }
    cout << endl;
}
