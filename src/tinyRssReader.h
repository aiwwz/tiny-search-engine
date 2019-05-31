#ifndef __RSSREADER_H__
#define __RSSREADER_H__
#include <string>
#include <vector>
using std::string;  using std::vector;

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

class RssReader{
public:
    RssReader() {}
    ~RssReader() {}

    bool parseRss(const char *xmlPath); //解析XML, 并将结果存入m_rss
    bool dump(const char *libname); //将m_rss中存放的解析内容按格式输出到文件中

private:
    vector<RssItem> m_rss; //存放解析结果
};

#endif /* __RSSREADER_H__ */

