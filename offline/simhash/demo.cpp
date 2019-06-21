#include <iostream>
#include <fstream>

//this define can avoid some logs which you don't need to care about.
#define LOGGER_LEVEL LL_WARN 

#include "Simhasher.hpp"
using namespace simhash;

int main(int argc, char** argv)
{
    Simhasher simhasher("/home/wzjj1314/github/cppjieba/dict/jieba.dict.utf8", "/home/wzjj1314/github/cppjieba/dict/hmm_model.utf8", "/home/wzjj1314/github/cppjieba/dict/idf.utf8", "/home/wzjj1314/github/cppjieba/dict/stop_words.utf8");

    string s1("我是蓝翔技工拖拉机学院手扶拖拉机专业的。不用多久，我就会升职加薪，当上总经理，出任CEO，走上人生巅峰。");
    string s2("我是蓝翔技工拖拉机学院手扶拖拉机专业的! 要不了多久，我就会升职加薪，当上总经理，出任CEO，走上人生巅峰。");
    size_t topN = 20;
    uint64_t u64_1 = 0, u64_2 = 0;
    vector<pair<string ,double> > res1, res2;
    simhasher.extract(s1, res1, topN);
    simhasher.extract(s2, res2, topN);
    simhasher.make(s1, topN, u64_1);
    simhasher.make(s2, topN, u64_2);
    cout << "文本1：\"" << s1 << "\"" << endl;
    cout << "simhash值是: " << u64_1 <<endl; 
    cout << endl;
    cout << "文本2：\"" << s2 << "\"" << endl;
    cout << "关键词序列是: " << res2 << endl;
    cout << "simhash值是: " << u64_2 <<endl; 
    cout << endl;
    cout << "两者是否相等:" << (Simhasher::isEqual(u64_1, u64_2, 3) ? "是" : "否") << endl;

/*
    const char * bin1 = "100010110110";
    const char * bin2 = "110001110011";
    uint64_t u1, u2;
    u1 = Simhasher::binaryStringToUint64(bin1); 
    u2 = Simhasher::binaryStringToUint64(bin2); 
    
    cout<< bin1 << "和" << bin2 << " simhash值的相等判断如下："<<endl;
    cout<< "海明距离阈值默认设置为3，则isEqual结果为：" << (Simhasher::isEqual(u1, u2)) << endl; 
    cout<< "海明距离阈值默认设置为5，则isEqual结果为：" << (Simhasher::isEqual(u1, u2, 5)) << endl; 
*/
    return EXIT_SUCCESS;
}
