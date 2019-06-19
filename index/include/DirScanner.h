/*********************************************
* file:   DirScanner.h
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-18 21:49:36
**********************************************/
#ifndef __DIRSCANNER_H__
#define __DIRSCANNER_H__
#include <vector>
#include <string>
using std::vector;  using std::string;
 
namespace tinyse {

class Configure;

class DirScanner {
public:
    DirScanner(Configure &config);

    const vector<string>& getCorpusFiles() const;
    void print() const; //for debug

private:
    void traverse(const string &corpusDir);

private:
    vector<string> m_vecCorpus;
    Configure &m_config;
};

} //end of namespace tinyse
 
#endif /* __DIRSCANNER_H__ */
