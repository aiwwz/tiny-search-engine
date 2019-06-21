#ifndef __RSSREADER_H__
#define __RSSREADER_H__
#include <string>
#include <vector>
using std::string;  using std::vector;

/*
struct RssItem { //存放<item>结点中的内容
    string title;
    string link;
    string content;

    void clear() {
        title.clear();
        link.clear();
        content.clear();
    }
};
*/

class Page;

class RssReader{
public:
    RssReader(vector<Page> &vecPages);

    bool parseRss(const string &pagePath); //解析XML, 并将结果存入m_vecPages
    bool dump(const char *libname); //将m_rss中存放的解析内容按格式输出到文件中

private:
    vector<Page> &m_vecPages; //存放解析结果
};

#endif /* __RSSREADER_H__ */

