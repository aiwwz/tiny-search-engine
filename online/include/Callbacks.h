/*********************************************
 * file:   Callbacks.h
 * author: AIWWZ(wzj1524@qq.com)
 * date:   2019-07-03 18:17:08
 **********************************************/
#ifndef __CALLBACKS_H__
#define __CALLBACKS_H__
#include <functional>
#include <memory>
#include <string>
#include "Timestamp.h"
using std::function;  using std::shared_ptr;  using std::string;

namespace tinyse {

class TcpConnection;
class Buffer;

using TcpConnectionPtr = shared_ptr<TcpConnection>;
using ConnectionCallback = function<void(const TcpConnectionPtr&)>;
//using MessageCallback = function<void(const TcpConnectionPtr&, Buffer*, Timestamp)>;
using MessageCallback = function<void(const TcpConnectionPtr&, char*, size_t)>;

} //end of namespace tinyse

#endif /* __CALLBACKS_H__ */
