/*********************************************
* file:   PageLib.h
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-18 21:45:55
**********************************************/
#ifndef __PAGELIB_H__
#define __PAGELIB_H__
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include "Page.h"
using std::vector;  using std::string;  using std::map;
using std::pair;  using std::unordered_map;

namespace tinyse {

class Configure;

class PageLib {
public:
    PageLib(Configure &config);
    void doit();

private:
    void load();  //加载并处理原始网页
    void dedup(); //页面去重
    void buildInvertedIndex(); //建立倒排索引
    void store(); //存储网页库/偏移库/倒排索引库

private:
    Configure &m_config; //配置
    vector<Page> m_vecPages; //存放格式化之后的网页
    unordered_map<string, vector<pair<size_t, double>>> m_invertedIndex; //倒排索引
};

} //end of namespace tinyse
 
#endif /* __PAGELIB_H__ */
