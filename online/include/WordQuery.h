/*********************************************
* file:   WordQuery.h
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-07-07 11:51:34
**********************************************/
#ifndef __WORDQUERY_H__ 
#define __WORDQUERY_H__ 
#include "WordSegmentation.h"
#include "Page.h"
#include <map>
using std::unordered_map;  using std::pair;
 
namespace tinyse {

class Configure;

/*  */
class WordQuery {
public:
    WordQuery(Configure &conf);
    string query(const string &str);  

private:
    void load();
    vector<double> getQueryWordsWeightVec(const vector<string> &queryWords);
    bool executeQuery(const vector<string> &queryWords, vector<pair<int, vector<double>>> &result);
    string createJson(const vector<int> &docidVec, const vector<string> &queryWords);
    string noResult(const string &str);

private:
    Configure &m_conf;
    WordSegmentation m_jieba; //分词器
    unordered_map<int, Page> m_pageLib; //网页库
    unordered_map<int, pair<int, int>> m_offsetLib; //<docid, <offset, length>>
    unordered_map<string, set<pair<int, double>>> m_invertedIndex; //倒排索引库
};

} //end of namespace tinyse
 
#endif /* __WORDQUERY_H__  */
