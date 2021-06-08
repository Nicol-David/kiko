#pragma once

#include "EventLoop.h"
#include "Channel.h"
#include "Acceptor.h"
#include "TcpConnection.h"
#include "Buffer.h"
#include "EventLoopThreadPool.h"

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

#define BUF_SIZE 1024

class TcpServer{
public:
    TcpServer(EventLoop *loop,int port);
    ~TcpServer();
    void start();
    void NewConnectCallback(TcpConnectionPtr newtcpconnection);
    void NewConnect(int connfd);
    void ReadClient(TcpConnectionPtr newtcpconnection,Buffer* thebuffer);

private:
    EventLoop* loop_;
    std::unique_ptr<Acceptor> acceptor_;
    //用于监听的acceptor
    std::map<std::string,TcpConnectionPtr> TcpConnectionMap;
    //所有连接的总维护
    std::shared_ptr<EventLoopThreadPool> threadPool_;
}