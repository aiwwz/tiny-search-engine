#ifndef __RSSREADER_H__
#define __RSSREADER_H__
#include <string>
#include <vector>
#include "Simhasher.hpp"
using std::string;  using std::vector;

namespace tinyse {

class Page;
class Configure;
class WordSegmentation;

class RssReader{
public:
    RssReader(vector<Page> &vecPages) : m_vecPages(vecPages) { }

    bool parseRss(const string &xmlPath, Configure &config, WordSegmentation &jieba, simhash::Simhasher &simhasher); //解析XML, 并将结果存入m_rss

private:
    vector<Page> &m_vecPages; //存放解析结果
};

} //end of namespace tinyse

#endif /* __RSSREADER_H__ */
