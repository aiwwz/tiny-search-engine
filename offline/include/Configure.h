/*********************************************
* file:   Configure.h
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-18 18:08:35
**********************************************/
#ifndef __CONFIGURE_H__
#define __CONFIGURE_H__
#include <string>
#include <map>
#include <set>
#include <fstream>
using std::string;  using std::map;  using std::ifstream;
using std::set;

namespace tinyse {

/* 读取配置文件 */
class Configure {
public:
    Configure(const string &filepath);
    ~Configure() { }

    map<string, string> getConfigMap(); //获取存放配置信息的map
    set<string>& getStopWords(); //获取停用词集
    void print() const; //for debug;

private:
    void defaultConfig(); //默认配置
    void loadConfig(ifstream &configs); //加载配置信息
    void readStopWords(); //读取停用词集

private:
    map<string, string> m_configMap; //数据文件-->存储路径
    set<string> m_stopWords; //停用词集
}; 

} //end of namespace tinyse 
 
#endif /* __CONFIGURE_H__ */
