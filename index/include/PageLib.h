/*********************************************
* file:   PageLib.h
* author: AIWWZ(wzj1524@qq.com)
* date:   2019-06-18 21:45:55
**********************************************/
#ifndef __PAGELIB_H__
#define __PAGELIB_H__

namespace tinyse {

class Configure;

class PageLib {
public:
    PageLib(Configure &config);

private:
    Configure &m_config;


};

} //end of namespace tinyse
 
#endif /* __PAGELIB_H__ */
