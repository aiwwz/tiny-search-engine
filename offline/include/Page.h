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
#include "Simhasher.hpp"
using std::string;  using std::vector;  using std::map;
using std::ostream;

 
namespace tinyse {

class Configure;
class WordSegmentation;

class Page {
    friend ostream& operator<<(ostream &os, const Page &page);
    friend bool operator==(const Page &lhs, const Page &rhs);
public:
    Page() { }

    void setDocID(const size_t &docid);
    void setTitle(const string &title);
    void setLink(const string &url);
    void setContent(const string &content);
    void parse(Configure &config, WordSegmentation &jieba, simhash::Simhasher &simhasher);
    map<string, size_t>& getWordsMap();
    size_t getDocID() const;
    string getTitle() const;
    string getContent() const;
    void clear();

private:
    size_t m_docid;
    string m_title;
    string m_link;
    string m_content;

    map<string, size_t> m_wordsMap; //分词(去停用词)之后的词:词频
    uint64_t m_simhash; //该Page的simhash值
};

} //end of namespace tinyse
 
#endif /* __PAGE_H__ */
