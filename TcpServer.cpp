
#include "TcpSever.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"
#include "Acceptor.h"
#include <stdio.h>
#include <functional>
#include <memory>
#include <vector>
#include "TcpConnection.h"
#include <string>
#include "EventLoopThreadPool.h"


typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

 TcpSever::TcpSever(EventLoop * loop, int port)
 	:loop_(loop),
 	acceptor_(new Acceptor(loop, port)),
 	threadPool_(new EventLoopThreadPool(loop, 5))
 	
 {
	acceptor_->setNewConnectionCallback(std::bind(&TcpSever::NewConnect, this, std::placeholders::_1));
	
	//printf("\nlistenfd_:%d\n", listenfd_);
	 //printf("\nlistenchannel_->getfd():%d\n", listenchannel_->getfd());

 }

TcpSever::~TcpSever(){}


void TcpSever::Start(){
	threadPool_->start();
    //线程池开始启动
	acceptor_->start();
    //监听事件开始启动
	return;
}

void TcpSever::ReadClient(TcpConnectionPtr newtcpconnection, Buffer* thebuffer ){

	std::string data = thebuffer->retrieveAllAsString();
	for(auto it = TcpConnectionMap.begin(); it != TcpConnectionMap.end(); it++){
		it->second->send(data);
	}
	printf("ReadClient\n");

}

void TcpSever::NewConnectCallback(TcpConnectionPtr newtcpconnection){
	printf("\n%s\n", newtcpconnection->getname().c_str());
}

 
void TcpSever::NewConnect(int connfd){//监听事件被触发时，将会执行此函数

    printf("connfd:%d\n", connfd);
    
    std::string nameArg = "tcpconnection" + std::to_string(connfd);
    EventLoop* ioLoop = threadPool_->getNextLoop();
    //获取线程池的loop
    TcpConnectionPtr newtcpconnection(new TcpConnection(ioLoop, nameArg, connfd));
    //建立TcpConnection类型的智能指针
    newtcpconnection->setMessageCallback(std::bind(&TcpSever::ReadClient, this, newtcpconnection, newtcpconnection->getintputbuffer()));
    //这是由setMessageCallback的参数MessageCallback来决定的，这里回调的是readclient函数
    newtcpconnection->setConnectionCallback(std::bind(&TcpSever::NewConnectCallback, this, newtcpconnection));
    
    
    TcpConnectionMap[nameArg] = newtcpconnection;
    ioLoop->queueInLoop(std::bind(&TcpConnection::connectEstablished, newtcpconnection));
    
}

