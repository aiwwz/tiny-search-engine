#include <regex>
#include "../include/RssReader.h"
#include "../include/tinyxml2.h"
#include "../include/MyLogger.h"
using namespace tinyxml2;
using std::to_string;


/* 将src中所有的<...>过滤掉 */
void regexFilter(const string &src, string &des) {
    string pattern = "<.*?>"; //.*?的目的是采用最短匹配
    std::regex reg(pattern);
    des = regex_replace(src, reg, ""); //将<与>之间包括<>全部替换为""
}

bool RssReader::parseRss(const string &xmlPath) {
    //加载待解析xml文件
    XMLDocument doc;
    if(XML_SUCCESS != doc.LoadFile(xmlPath.c_str())) {
        LogWarn("Load XML file failed!");
        return false;
    }
    
    XMLElement *rssRoot = doc.RootElement(); //根节点<rss>

    XMLElement *channel = rssRoot->FirstChildElement("channel"); //<channel>

    XMLElement *item = channel->FirstChildElement("item"); //<item>

    RssItem tmpItem; //临时存放<item>结点中的数据
    XMLElement *tmpNode; //指向<item>的子结点

    while(item != nullptr) { //获取指定结点的内容, 并存入m_rss中
        tmpNode = item->FirstChildElement("title");
        if(nullptr != tmpNode) {
            tmpItem.title = tmpNode->GetText();
        }

        tmpNode = item->FirstChildElement("link");
        if(nullptr != tmpNode) {
            tmpItem.link = tmpNode->GetText();
        }

        tmpNode = item->FirstChildElement("content:encoded");
        if(nullptr == tmpNode) { //若没有content, 则用description代替之
            tmpNode = item->FirstChildElement("description");
        }
        if(nullptr != tmpNode) {
            //使用正则表达式过滤无用的数据
            regexFilter(tmpNode->GetText(), tmpItem.content); 
        }

        m_rss.push_back(tmpItem);
        tmpItem.clear();
         
        item = item->NextSiblingElement("item"); //继续解析下一个<item>结点
    }

    return true;
}


/* 存储到容器: 通过原生string方式 */
void RssReader::store(vector<string> &vecPages) {
    string page;
    size_t size = m_rss.size();
    for(size_t idx = 0; idx != size; ++idx) {
        page = "<doc>\n    <docid>" 
             + to_string(idx+1) + "</docid>\n    <title>"
             + m_rss[idx].title + "</title>\n    <link>"
             + m_rss[idx].link + "</link>\n    <content>"
             + m_rss[idx].content + "\n</content>\n</doc>\n\n";
        vecPages.push_back(page);
    }
}

/* 存储到文件: 通过tinyxml2提供的接口
bool RssReader::store(const string &libPath) {
    XMLDocument docs;
   
    XMLElement *node;
    XMLElement *doc;
    unsigned int docid = 1;
    for(auto item : m_rss) {
        doc = docs.NewElement("doc"); //新建doc结点
        docs.InsertEndChild(doc);

        node = docs.NewElement("docid"); //新建docid结点并插入doc结点中
        node->SetText(docid++);
        doc->InsertEndChild(node);

        node = docs.NewElement("title"); //新建title结点并插入doc结点中
        node->SetText(item.title.c_str());
        doc->InsertEndChild(node);

        node = docs.NewElement("link"); //新建link结点并插入doc结点中
        node->SetText(item.link.c_str());
        doc->InsertEndChild(node);

        node = docs.NewElement("content"); //新建content结点并插入doc结点中
        node->SetText(item.content.c_str());
        doc->InsertEndChild(node);
    }
    if(XML_SUCCESS ==docs.SaveFile(libPath.c_str())) {
        return true;
    }
    else {
        LogWarn("Save XML file faild!");
        return false;
    }
}
*/
