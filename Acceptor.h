#pragma once
#include "Channel.h"
#include "Socket.h"


typedef std::shared_ptr<channel> SP_Channel;

class Acceptor{
public:
    typedef std::function<void (int sockfd)> NewConnectionCallback;
    Acceptor(EventLoop* loop,int port);
    ~Acceptor();
    void start();
    //开始监听
    int getport() const {return port_;}
    int getlistenfd() const {return listenfd_;}
    SP_Channel getlistenchannel() const {return listenchannel_;}
    void setNewConnectionCallback(const NewConnectionCallback& cb)
    {
        NewConnectionCallback_=cb;
    }

private:
    void handleRead();
    //listenchannel_的可读函数句柄
    EventLoop* loop_;
    //类内成员的初始化存在顺序
    int port_;
    //tcpserver监听的端口号
    int listenfd_;
    //tcpserver所建立的文件描述符
    SP_Channel listenchannel_;
    //tcpserver用来监听的channel
    NewConnectionCallback NewConnectionCallback_;
    //有新连接时，回调此函数
};