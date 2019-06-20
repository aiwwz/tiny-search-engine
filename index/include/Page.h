/*********************************************
* file:   Page.h
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-19 18:15:15
**********************************************/
#ifndef __PAGE_H__
#define __PAGE_H__
#include <string>
#include <vector>
#include <map>
#include <ostream>
using std::string;  using std::vector;  using std::map;
using std::ostream;

 
namespace tinyse {

class Configure;
class WordSegmentation;

class Page {
    friend ostream& operator<<(ostream &os, const Page &page);
public:
    Page(Configure &config);

    void setDocID(const size_t &docid);
    void setTitle(const string &title);
    void setUrl(const string &url);
    void setContent(const string &content);
    void parse();

private:
    Configure &m_config;
    WordSegmentation &m_wordSeg;

    size_t m_docid;
    string m_title;
    string m_url;
    string m_content;

    map<string, size_t> m_wordsMap; //分词(去停用词)之后的词:词频
    vector<string> m_topKWords;     //该Page的特征向量
};

} //end of namespace tinyse
 
#endif /* __PAGE_H__ */
