#include "Acceptor.h"

Acceptor::Acceptor(EventLoop* loop,int port)
:loop_(loop),
port_(port),
listenfd_(socket::SocketBind(port_)),
//创建套接字描述符
listenchannel_(new Channel(loop_,listenfd_)){
    listenchannel_->SetReadHandl(std::bind(&Acceptor::handleRead,this));
    //void Channel::SetReadHandl(CallBack&&  ReadHandler){ReadHandler_ = ReadHandler;};
    //这里的表达为 ReadHandler=std::bind(&Acceptor::handleRead,this)，好理解了吧
}

Acceptor::~Acceptor(){}

void Acceptor::handleRead(){
    struct sockaddr_in client;
    socklen_t client_addrlength=sizeof(client);
    int connfd=accept(listenfd_,(struct sockaddr*)&client,&client_addrlength);
    if(connfd>0){
        if(NewConnectionCallback_){
            NewConnectionCallback_(connfd);
        }
    }
}

void Acceptor::start(){
    listenchannel_->enableReading();
}