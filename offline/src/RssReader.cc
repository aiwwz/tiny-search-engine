#include <regex>
#include "../include/RssReader.h"
#include "../include/tinyxml2.h"
#include "../include/MyLogger.h"
#include "../include/Page.h"
#include "../include/Configure.h"
#include "../include/WordSegmentation.h"
using namespace tinyxml2;
using namespace tinyse;
using std::to_string;


/* 将src中所有的<...>过滤掉 */
string regexFilter(const string &src) {
    string pattern = "<.*?>"; //.*?的目的是采用最短匹配
    std::regex reg(pattern);
    return regex_replace(src, reg, ""); //将<与>之间包括<>全部替换为""
}


bool RssReader::parseRss(const string &xmlPath, Configure &config, WordSegmentation &jieba, simhash::Simhasher &simhasher) {
    //加载待解析xml文件
    XMLDocument doc;
    if(XML_SUCCESS != doc.LoadFile(xmlPath.c_str())) {
        LogWarn("Load rss file failed!");
        return false;
    }
    
    XMLElement *rssRoot = doc.RootElement(); //根节点<rss>

    XMLElement *channel = rssRoot->FirstChildElement("channel"); //<channel>

    XMLElement *item = channel->FirstChildElement("item"); //<item>

    Page tmpPage; //临时存放<item>结点中的数据
    XMLElement *tmpNode; //指向<item>的子结点

    while(item != nullptr) { //获取指定结点的内容, 并存入m_rss中
        tmpNode = item->FirstChildElement("title");
        if(nullptr != tmpNode) {
            tmpPage.setTitle(tmpNode->GetText());
        }
        else { //忽略不完整Page
            continue;
        }

        tmpNode = item->FirstChildElement("link");
        if(nullptr != tmpNode) {
            tmpPage.setLink(tmpNode->GetText());
        }
        else { //忽略不完整Page
            continue;
        }

        tmpNode = item->FirstChildElement("content:encoded");
        if(nullptr == tmpNode) { //若没有content, 则用description代替之
            tmpNode = item->FirstChildElement("description");
        }
        if(nullptr != tmpNode) {
            //使用正则表达式过滤无用的数据
            tmpPage.setContent(regexFilter(tmpNode->GetText())); 
        }
        else { //忽略不完整Page
            continue;
        }

        tmpPage.parse(config, jieba, simhasher); //该Page已格式化, 进行进一步处理

        m_vecPages.push_back(tmpPage);
        tmpPage.clear();

        item = item->NextSiblingElement("item"); //继续解析下一个<item>结点
    }

    return true;
}
