#pragma once
#include <functional>
#include <memory>
#include "Buffer.h"
#include "TcpConnection.h"

class TcpConnection;
class Buffer;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;


//此文件记录了所有的回调函数的定义

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;
//一个新的tcpconnection建立时的回调函数
typedef std::function<void (const TcpConnectionPtr&, Buffer*)> MessageCallback;
//tcpconnection收到消息时的回调函数