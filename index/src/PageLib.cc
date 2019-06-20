/*********************************************
* file:   PageLib.cc
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-19 09:30:40
**********************************************/
#include "../include/PageLib.h"
#include "../include/Configure.h"
#include "../include/DirScanner.h"
#include "../include/RssReader.h"
#include "../include/MyLogger.h"
#include <fstream>
using namespace tinyse;
using std::ofstream;

PageLib::PageLib(Configure &config, DirScanner &dirScanner) 
    : m_config(config)
    , m_dirScanner(dirScanner) { 

        rssReader.store(m_vecPages);
}























void PageLib::doit() { 
   load();
   dedup();
   buildInvertedIndex();
   store();
}
/***************************************************************/
void PageLib::load() {
    /* 网页存放在哪里? */
    DirScanner scanner(m_config);
    const vector<string> &corpusPages = scanner.getCorpusPages(); //获取原始网页存储路径

    /* 解析原始网页 --> 生成格式化的网页 */
    RssReader rssReader;
    for(auto &pagePath : corpusPages) {
        rssReader.parseRss(pagePath, m_vecPages);
        LogInfo("parse rss page: %s", pagePath.c_str());
    }
       
}












/* 存储格式化的网页库/偏移库/倒排索引 */
void PageLib::store() {
    ofstream pageLib(m_config.getConfigMap()["ripepagelib"]);
    ofstream offsetLib(m_config.getConfigMap()["offsetlib"]);
    if(!pageLib || !offsetLib) {
        LogWarn("Can't open output lib file!");
        exit(-1);
    }

    size_t offset, length;
    size_t size = m_vecPages.size();
    for(size_t idx = 0; idx != size; ++idx) {
        offset = pageLib.tellp();
        length = m_vecPages[idx].size();
        offsetLib << idx + 1 << "\t" << offset << "\t" << length << std::endl; //存储偏移
        pageLib << m_vecPages[idx]; //存储网页
    }
    LogInfo("Store page lib & offset lib!");
}

