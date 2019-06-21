/*********************************************
 * file:   WordSegmentation.h
 * author: AIWWZ(wzj1524@qq.com)
 * date:   2019-06-19 21:48:01
 **********************************************/
#ifndef __WORDSEGMENTATION_H__
#define __WORDSEGMENTATION_H__
#include "cppjieba/Jieba.hpp"
#include "MyLogger.h"
#include <vector>
#include <string>
using std::vector;  using std::string;

const char* const DICT_PATH = "/home/wzjj1314/github/cppjieba/dict/jieba.dict.utf8";                    
const char* const HMM_PATH = "/home/wzjj1314/github/cppjieba/dict/hmm_model.utf8";
const char* const USER_DICT_PATH = "/home/wzjj1314/github/cppjieba/dict/user.dict.utf8";
const char* const IDF_PATH = "/home/wzjj1314/github/cppjieba/dict/idf.utf8";
const char* const STOP_WORD_PATH = "/home/wzjj1314/github/cppjieba/dict/stop_words.utf8";

namespace tinyse {

class WordSegmentation {
public:
    WordSegmentation() : m_jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH) {
        LogInfo("cppjieba init!");
    } 

    vector<string> operator()(const string &src) {
        vector<string> words;
        m_jieba.CutAll(src, words);
        return words;
    }

private:
    cppjieba::Jieba m_jieba;
};

} //end of namespace tinyse

#endif /* __WORDSEGMENTATION_H__ */
