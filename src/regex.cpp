#include <iostream>
#include <regex>
#include <string>
using std::cout; using std::endl; 
using std::string; using std::smatch; 
using std::regex; using std::sregex_iterator;

int main() {
    string str = "本篇文章会覆盖如下技术：<dsfadfa&lt/p> <fsdaful> <fsdfali>HTTP Basic</fasdfli> <lfasdfasdfi>Digest Access"; 
    string pattern = "<.*?>";
    regex r(pattern);
    string fmt = "";

    cout << regex_replace(str, r, "") << endl;

    //smatch results;

    //for(sregex_iterator it(str.begin(), str.end(), r), end_it; it != end_it; ++it) {
    //    cout << it->str() << endl;
    //}



    //int ret = regex_search(str, results, r);
    //cout << "ret=" << ret << endl;
    //if(ret) {
    //    cout << results.str() << endl;
    //}

    return 0;
}

