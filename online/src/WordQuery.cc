/*********************************************
 * file:   WordQuery.cc
 * author: AIWWZ(wzj1524@qq.com)
 * date:   2019-07-07 12:06:30
 **********************************************/
#include "../include/WordQuery.h"
#include "../include/Configure.h"
#include "../include/MyLogger.h"
#include <json/json.h>
#include <fstream>
#include <sstream>
using namespace tinyse;
using std::ifstream;  using std::ofstream; using std::stringstream; 
using std::make_pair;

WordQuery::WordQuery(Configure &conf) 
    : m_conf(conf)
    , m_jieba() {

    LogDebug("1");
    load(); //加载各类数据
    LogDebug("2");
}

void WordQuery::load() {
    //加载网页库&网页偏移库
    ifstream pageLib(m_conf.getConfigMap()["pagelib"]);
    ifstream offsetLib(m_conf.getConfigMap()["offsetlib"]);
    if(!pageLib.good() || !offsetLib.good()) {
        if(!pageLib.good()) {
            LogError("open pagelib fail!");
        }
        else if(!offsetLib.good()) {
            LogError("open offsetlib fail!");
        }
        else {
            LogError("open pagelib & offsetlib fail!");
        }
        exit(-1);
    }

    int docid, offset, length;
    string line;
    while(std::getline(offsetLib, line)) {
        stringstream record(line);
        record >> docid >> offset >> length;
        string doc(length, '\0'); //必须提前申请足额空间
        pageLib.seekg(offset, pageLib.beg);
        pageLib.read(&*doc.begin(), length);

        Page page(doc, m_conf, m_jieba);
        m_pageLib.insert(make_pair(docid, page));
        m_offsetLib.insert(make_pair(docid, make_pair(offset, length)));
    }

    pageLib.close();
    offsetLib.close();

    //加载倒排索引
    ifstream invertedIndexLib(m_conf.getConfigMap()["invertedindexlib"]);
    if(!invertedIndexLib.good()){
        LogError("open invertedIndexLib fail!");
        exit(-1);
    }

    string word;
    double weight;
    while(std::getline(invertedIndexLib, line)) {
        stringstream record(line);
        record >> word;
        set<pair<int, double>> tmp; //<docid, weight>
        while(record >> docid >> weight) {
            tmp.insert(make_pair(docid, weight));
        }
        m_invertedIndex.insert(make_pair(word, tmp));
    }

    invertedIndexLib.close();
    LogInfo("load lib success!");
}

class Compare {
public:
    Compare(vector<double> &base) : m_base(base) { }

    //基于余弦相似度的比较
    bool operator()(const pair<int, vector<double>> &lhs, const pair<int, vector<double>> &rhs) {
        double lhsCrossProduct = 0;
        double rhsCrossProduct = 0;
        double lhsVectorLength = 0;
        double rhsVectorLength = 0;

        for(size_t index = 0; index != m_base.size(); ++index) {
            lhsCrossProduct += (lhs.second)[index] * m_base[index];
            rhsCrossProduct += (rhs.second)[index] * m_base[index];
            lhsVectorLength += pow((lhs.second)[index], 2);
            rhsVectorLength += pow((rhs.second)[index], 2);
        }

        if(lhsCrossProduct / sqrt(lhsVectorLength) < rhsCrossProduct / sqrt(rhsVectorLength)) {
            return false;
        }
        else {
            return true;
        }
    }
private:
    vector<double> m_base; //基准向量
};

string WordQuery::query(const string &str) {
    //对待查询字符串分词
    vector<string> queryWords;
    queryWords = m_jieba(str);

    string words; //for debug
    for(auto &it : queryWords) {
        words.append(it + ", ");
    }

    for(auto &it : queryWords) {
        if(m_invertedIndex.find(it) == m_invertedIndex.end()) {
            LogInfo("noResult: %s", words.c_str());
            return noResult(str);
        }
    }

    //计算每个待查询词的weight
    vector<double> weightVec = getQueryWordsWeightVec(queryWords);

    //查询
    vector<pair<int, vector<double>>> resultVec; //<docid, 关键词在该page中的权重系数vec>
    if(executeQuery(queryWords, resultVec)) {
        //按相关性对获取的网页进行排序
        Compare cmp(weightVec);
        stable_sort(resultVec.begin(), resultVec.end(), cmp);

        vector<int> docidVec;
        for(auto &it : resultVec) {
            docidVec.push_back(it.first);
        }

        //将查询结果封装成Json数据
        return createJson(docidVec, queryWords);
    }
    else {
        return noResult(str);
    }
}


string WordQuery::noResult(const string &str) {
    Json::Value root;
    Json::Value arr;
    Json::Value elem;

    elem["title"] = "404, not found";
    string summary = "很抱歉, 没有找到与" + str + "相关的网页";
    elem["summary"] = summary;
    elem["url"] = "";
    arr.append(elem);
    root["files"] = arr;
    Json::StyledWriter writer;
    return writer.write(root);
}


/* 获取待查询词的权重系数 */
vector<double> WordQuery::getQueryWordsWeightVec(const vector<string> &queryWords) {
    //统计词频
    map<string, int> wordFreqMap;
    for(auto &it : queryWords) {
        ++wordFreqMap[it];
    }

    //使用TF-IDF算法计算权重系数
    vector<double> weightVec; 
    double sumOfSquares = 0;  //所有词语的权重的平方和

    int TF = 0, DF = 0;
    double IDF = 0.0, W = 0.0;
    int N = m_pageLib.size(); //page总数
    for(auto &it : queryWords) {
        TF = wordFreqMap[it];  //词频
        DF = m_invertedIndex[it].size(); //包含改词的文档数
        IDF = log2(static_cast<double>(N+0.1) / DF); //逆文档频率(防止DF==N)
        W = TF * IDF; //权重系数
        sumOfSquares += W * W;
        weightVec.push_back(W);
    }

    //对权重系数归一化处理
    for(auto &it : weightVec) {
        it /= sqrt(sumOfSquares);
    }

    return weightVec;
}

/* 执行查询操作 */
bool WordQuery::executeQuery(const vector<string> &queryWords, vector<pair<int, vector<double>>> &resultVec) {
    if(queryWords.size() == 0) {
        return false;
    }

    using setIter = set<pair<int, double>>::iterator;
    vector<pair<setIter, int>> iterVec; //保存需要取交集的迭代器
    int minIterNum = 0x7ffffff;
    for(auto &it : queryWords) {
        int size = m_invertedIndex[it].size();
        if(size == 0) {
            return false;
        }
        if(minIterNum > size) {
            minIterNum = size;
        }
        iterVec.push_back(make_pair(m_invertedIndex[it].begin(), 0));
    }

    bool isExiting = false;
    while(!isExiting) {
        size_t idx = 0;
        for(; idx != iterVec.size() - 1; ++idx)
        {
            if((iterVec[idx].first)->first != iterVec[idx+1].first->first)
                break;

        }

        if(idx == iterVec.size() - 1)
        {   // 找到相同的docid
            vector<double> weightVec;
            int docid = iterVec[0].first->first;
            for(idx = 0; idx != iterVec.size(); ++idx)
            {
                weightVec.push_back(iterVec[idx].first->second);
                ++(iterVec[idx].first);//相同时，将每一个迭代器++
                ++(iterVec[idx].second);//同时记录迭代器++的次数
                if(iterVec[idx].second == minIterNum)
                {   isExiting = true;   }

            }
            resultVec.push_back(make_pair(docid, weightVec));
        }
        else
        {   //找到最小的docid，并将其所在的iter++
            int minDocId = 0x7FFFFFFF;
            int iterIdx;//保存minDocId的迭代器位置
            for(idx = 0; idx != iterVec.size(); ++idx)
            {
                if(iterVec[idx].first->first < minDocId)
                {
                    minDocId = iterVec[idx].first->first;
                    iterIdx = idx;

                }

            }

            ++(iterVec[iterIdx].first);
            ++(iterVec[iterIdx].second);
            if(iterVec[iterIdx].second == minIterNum)
            {   isExiting = true;   }
        }

    }
    return true;
}


/* 封装Json数据包 */
string WordQuery::createJson(const vector<int> &docidVec, const vector<string> &queryWords) {
    Json::Value root;
    Json::Value arr;

    int cnt = 0;
    for(auto docid : docidVec) {
        Json::Value elem;
        elem["title"] = m_pageLib[docid].title();
        elem["url"] = m_pageLib[docid].link();
        elem["summary"] = m_pageLib[docid].summary(queryWords);
        arr.append(elem);
        if(++cnt == 100) { //最多100条
            break;
        }
    }

    root["files"] = arr;
    Json::StyledWriter writer;
    return writer.write(root);
}
