/*********************************************
* file:   Page.h
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-07-06 00:29:44
**********************************************/
#ifndef __PAGE_H__
#define __PAGE_H__
#include <vector>
#include <string>
#include <map>
#include <set>
using std::vector;  using std::string; 
using std::map;  using std::set;
 
namespace tinyse {

class Configure;
class WordSegmentation;

class Page {
public:
    Page() { }
    Page(const string &doc, Configure &conf, WordSegmentation &jieba);

    int docid() const { return m_docid; }
    string title() const { return m_title; }
    string link() const { return m_link; }
    string summary(const vector<string> &queryWords);

private:
    void parseDoc(const string &doc, Configure &conf, WordSegmentation &jieba);
    void calcTopK(vector<string> &wordVec, size_t K, set<string> &stopWords);
    const static size_t topK = 20;

private:
    int m_docid;
    string m_title;
    string m_link;
    string m_content;
    string m_summary;

    vector<string> m_topWords;   //topK词
    map<string, int> m_wordsMap; //<词, 频次>
};

} //end of namespacei tinyse
 
#endif /* __PAGE_H__ */
