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
#include "../include/WordSegmentation.h"
#include "../include/Simhasher.hpp"
#include <cmath> //for log2()
#include <fstream>
#include <iostream>
using namespace tinyse;
using std::ofstream;  using std::cout;  using std::endl;

PageLib::PageLib(Configure &config) : m_config(config) {
    LogInfo("PageLib init!");
}

void PageLib::doit() { 
   load();
   dedup();
   buildInvertedIndex();
   store();
}

/* 加载并处理原始网页 */
void PageLib::load() {
    /* 网页存放在哪里? --> 获取原始网页存储路径 */
    DirScanner scanner(m_config);
    const vector<string> &corpusPages = scanner.getCorpusPages(); 

    /* 解析原始网页 --> 生成格式化的网页 */
    RssReader rssReader(m_vecPages);
    WordSegmentation jieba;
    simhash::Simhasher simhasher("/home/wzjj1314/github/cppjieba/dict/jieba.dict.utf8", "/home/wzjj1314/github/cppjieba/dict/hmm_model.utf8", "/home/wzjj1314/github/cppjieba/dict/idf.utf8", "/home/wzjj1314/github/cppjieba/dict/stop_words.utf8");

    for(auto &pagePath : corpusPages) {
        LogInfo("parse rss page: %s", pagePath.c_str());
        rssReader.parseRss(pagePath, m_config, jieba, simhasher);
    }

    /* 设置docid */
    size_t docid = 0;
    for(auto &page : m_vecPages) {
        page.setDocID(++docid);
    }

    LogInfo("Load corpus pages success!");
}

/* 页面去重 */
void PageLib::dedup() {
    /* 时间复杂度O(N^2) --> 待优化 */
    for(size_t i = 0; i < m_vecPages.size(); ++i) {
        for(size_t j = i + 1; j < m_vecPages.size(); ++j) {
            if(m_vecPages[i] == m_vecPages[j]) {
                /* 两Page相似 --> 删除docid大的 */
                LogInfo("Delete duplicate page: <%s> with <%s>", m_vecPages[i].getTitle().c_str(), m_vecPages[j].getTitle().c_str());
                cout << m_vecPages[i].getDocID() << " == " << m_vecPages[j].getDocID() << endl;
                m_vecPages[j] = m_vecPages.back();
                m_vecPages.pop_back();
                --j;
            }
        }
    }
}

/* 建立倒排索引 */
void PageLib::buildInvertedIndex() {
    /* 读取倒排索引 --> 初步:以词频为权重 */
    for(auto &page : m_vecPages) {
        map<string, size_t> &wordsMap = page.getWordsMap();
        for(auto &it : wordsMap) {
            /* <word, <<id1, freq1>, <id2, freq2>, ...>> */
            m_invertedIndex[it.first].push_back(std::make_pair(page.getDocID(), it.second));
        }
    }
 
    /* 使用TF-IDF算法计算权重系数 */
    map<size_t, double> sumOfSquares; //一篇文档中所有词语权重的平方和

    size_t TF = 0, DF = 0;
    double IDF = 0.0, W = 0;
    size_t N = m_vecPages.size(); //Page总数
    for(auto &index : m_invertedIndex) {
        DF = index.second.size(); //包含该词的Page数量
        IDF = ::log2((N+0.1) / DF); //逆文档频率(防止DF==N)
        for(auto &it : index.second) {
            TF = it.second; //该词在该Page中出现的次数
            W = TF * IDF; //权重系数
            it.second = W; //更新权重系数
            sumOfSquares[it.first] += W * W; //求权重平方和
        }
    }

    /* 对权重系数归一化处理 */
    for(auto &index : m_invertedIndex) {
        for(auto &it : index.second) {
            it.second = it.second / ::sqrt(sumOfSquares[it.first]);
        }
    }
    LogInfo("Build inverted index success!");
}

/* 存储格式化的网页库/偏移库/倒排索引 */
void PageLib::store() {
    ofstream pageLib(m_config.getConfigMap()["pagelib"]);
    ofstream offsetLib(m_config.getConfigMap()["offsetlib"]);
    ofstream invertedIndexLib(m_config.getConfigMap()["invertedindexlib"]);
    if(!pageLib || !offsetLib || !invertedIndexLib) {
        LogWarn("Can't open output lib file!");
        exit(-1);
    }

    /* 存储网页库/偏移库 */
    size_t offset_new = 0, offset_old = 0;
    for(auto &page : m_vecPages) {
        pageLib << page; 
        offset_new = pageLib.tellp();
        offsetLib << page.getDocID() << "\t" << offset_old << "\t" << offset_new - offset_old << endl;
        offset_old = offset_new;
    }
    
    /* 存储倒排索引 */
    for(auto &index : m_invertedIndex) {
        invertedIndexLib << index.first << "\t";
        for(auto &it : index.second) {
            invertedIndexLib << it.first << " " << it.second << " "; 
        }
        invertedIndexLib << endl;
    }

    LogInfo("Store page lib & offset & inverted index lib!");
}
