/*********************************************
* file:   Page.cc
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-20 00:00:23
**********************************************/
#include "../include/Page.h"
#include "../include/Configure.h"
#include "../include/WordSegmentation.h"
#include "../include/MyLogger.h"
#include <iostream>
using std::cout;  using std::endl;

namespace tinyse {

void Page::setDocID(const size_t &docid) {
    m_docid = docid;
}

void Page::setTitle(const string &title) {
    m_title = title;
}

void Page::setLink(const string &link) {
    m_link = link;
}

void Page::setContent(const string &content) {
    m_content = content;
}

void Page::parse(Configure &config, WordSegmentation &jieba, simhash::Simhasher &simhasher) {
    /* 对content进行分词 */
    vector<string> vecWords = jieba(m_content); 

    /* 统计词频  --> 去掉单字词和停用词 */
    set<string> &stopWords = config.getStopWords();
    for(auto &word : vecWords) {
        if(word.size() > 1 && stopWords.find(word) == stopWords.end()) {
            ++m_wordsMap[word];
        }
    }

    /* 求该Page的simhash */
    size_t topN = 20; //默认为10
    simhasher.make(m_content, topN, m_simhash); //计算simhash
    /*
    vector<std::pair<string, double>> res;
    simhasher.extract(m_content, res, topN);
    for(auto &it : res) {
        cout << "[" << it.first << ":" << it.second << "] ";
    }
    cout << endl;
    */
}

map<string, size_t>& Page::getWordsMap() {
    return m_wordsMap;
}

size_t Page::getDocID() const {
    return m_docid;
}

string Page::getTitle() const {
    return m_title;
}

string Page::getContent() const {
    return m_content;
}

void Page::clear() {
    m_docid = 0;
    m_title.clear();
    m_link.clear();
    m_content.clear();
    m_wordsMap.clear();
    m_simhash = 0;
}

/* 赋值 --> 仅修改内容, 不改动docid */
void Page::operator=(const Page &rhs) {
    m_title = rhs.m_title;
    m_link = rhs.m_link;
    m_content = rhs.m_content;
    m_wordsMap = rhs.m_wordsMap;
    m_simhash = rhs.m_simhash;
}
ostream& operator<<(ostream &os, const Page &page) {
    os << "<doc>\n"
       << "    <docid>"   << page.m_docid   << "</docid>\n"
       << "    <title>"   << page.m_title   << "</title>\n"
       << "    <link>"    << page.m_link    << "</link>\n"
       << "    <content>" << page.m_content << "</content>\n"
       << "</doc>\n";
    return os;
}

/* 判断两个Page是否相似 */
bool operator==(const Page &lhs, const Page &rhs) {
    return simhash::Simhasher::isEqual(lhs.m_simhash, rhs.m_simhash); //海明距离阈值默认值为3
    
    return true;
}

} //end of namespace tinyse
