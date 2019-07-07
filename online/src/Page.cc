/*********************************************
* file:   Page.cc
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-07-06 15:20:40
**********************************************/
#include "../include/Page.h"
#include "../include/Configure.h"
#include "../include/WordSegmentation.h"
#include <queue>
#include <istream>
using namespace tinyse;
using std::priority_queue;  using std::pair;
using std::istringstream;

Page::Page(const string &doc, Configure &conf, WordSegmentation &jieba) {
    parseDoc(doc, conf, jieba);
}

void Page::parseDoc(const string &doc, //包含标记的完整<doc>结点
                    Configure &conf,   //获取配置信息
                    WordSegmentation &jieba) { //jieba分词器
    int beg = 0, end = 0;
    size_t len = 0;

    //获取docid
    beg = doc.find("<docid>");
    end = doc.find("</docid>");
    len = strlen("<docid>");
    string docid = doc.substr(beg+len, end-beg-len);
    m_docid = atoi(docid.c_str());

    //获取title
    beg = doc.find("<title>");
    end = doc.find("</title>");
    len = strlen("<title>");
    m_title = doc.substr(beg+len, end-beg-len);

    //获取link
    beg = doc.find("<link>");
    end = doc.find("</link>");
    len = strlen("<link>");
    m_link = doc.substr(beg+len, end-beg-len);

    //获取content
    beg = doc.find("<content>");
    end = doc.find("</content>");
    len = strlen("<content>");
    m_content = doc.substr(beg+len, end-beg-len);
    
    //分词
    vector<string> wordVec = jieba(m_content);
    set<string> &stopWords = conf.getStopWords();

    //计算topK
    calcTopK(wordVec, topK, stopWords);
}

class WordFreqCompare {
public:
    bool operator()(const pair<string, int> &lhs, const pair<string, int> &rhs) {
        if(lhs.second < rhs.second) {
            return true;
        }
        else if(lhs.second == rhs.second && lhs.first < rhs.first){
            return true;
        }
        else {
            return false;
        }
    }
};

void Page::calcTopK(vector<string> &wordVec, size_t topK, set<string> &stopWords) {
    //去停用词&获取词频
    for(auto it : wordVec) {
        if(stopWords.find(it) == stopWords.end()) {
            ++m_wordsMap[it]; //非停用词, 词频++
        }
    } 

    //获取topK
    priority_queue<pair<string, int>, vector<pair<string, int>>, WordFreqCompare> heap(m_wordsMap.begin(), m_wordsMap.end());
    string topWord;
    while(!heap.empty() && m_topWords.size() < topK) { //获取词频最高的K个word
        topWord = heap.top().first; 
        heap.pop();
        m_topWords.push_back(topWord);
    }
}


/* 获取所查询词的摘要信息 */
string Page::summary(const vector<string> &queryWords) {
    vector<string> summaryVec;
    istringstream is(m_content);
    string line;
    while(std::getline(is, line)) {
        for(auto &word : queryWords) {
            if(line.find(word) != string::npos && summaryVec.size() < 3) {
                summaryVec.push_back(line);
            }
        }
    }

    string summary;
    for(auto &line : summaryVec) {
        summary.append(line);
    }

    return summary;
}


bool operator<(const Page &lhs, const Page &rhs) {
    return lhs.docid() < rhs.docid();
}
