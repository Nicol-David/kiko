#pragma once
#include "EventLoop.h"

#include <stdio.h>
#include <memory>
#include <functional>
#include <Epoll.h>


class EventLoop;


#define kNoneEvent  0;
#define kReadEvent  POLLIN | POLLPRI;
#define kWriteEvent  POLLOUT;


class Channel{
public:
	typedef std::function<void()> CallBack;

public:
	Channel(EventLoop* loop, int fd, int event);
	Channel(EventLoop* loop, int fd);
	~Channel();
	int getfd();
	int getEvents();
	void SetReadHandl(CallBack&&  ReadHandler){ReadHandler_ = ReadHandler;};
	void SetWriteHandl(CallBack&& WriteHandler){WriteHandler_ = WriteHandler;};
	void SetCloseHandl(CallBack&& CloseHandler){CloseHandler_ = CloseHandler;};
	void SetErrorHandl(CallBack&& ErroeHandler){ErroeHandler_ = ErroeHandler;};
    //参数中的&&写法，对于私有成员变量进行赋值，试着句柄
	//void SetMessageCallback(CallBack && MessageCallback){MessageCallback_ = MessageCallback;};

	void ReadHandl();
	void WriteHandl();
	void CloseHandl();
	void ErrorHandl();
	void MessageCallback();

	void EventHandl();
    //处理channel回调函数的接口函数，这里也是整个channel类的核心事件

	void SetEvent(int event){event_ = event;};
    //event_作为epoll_event的成员，可以保存的常量及指向的事件类型为EPOLLIN,EPOLLOUT等
	void SetRevent(int revent){revent_ = revent;};
	bool getflag(){return   flag_; };
	void setflag(bool flag){flag_ = flag; };

	void updata();
    //更新channel在epoll中的状态
	void remove();
    //将channel从epoll中删除

	//这些都是使能函数，根据不同的连接请求完成在epoll中event的更新
	void enableReading() { event_ |= kReadEvent; updata(); }
	void disableReading() { event_ &= ~kReadEvent; updata(); }
	void enableWriting() { event_ |= kWriteEvent; updata(); }
	void disableWriting() { event_ &= ~kWriteEvent; updata(); }
	void disableAll() { event_ = kNoneEvent; updata(); }
	//判断channel是否可以读写,在有需要时才来关注writable事件
	bool isWriting() const { return event_ & kWriteEvent; }
    bool isReading() const { return event_ & kReadEvent; }
	
private:
	int fd_;
	int event_;
    //channel关注的事件，由用户设置
	int revent_;
    //epoll返回的事件类型，是目前活动的事件
	EventLoop* loop_; 
    //对于当前的channel所属的eventloop
	CallBack ReadHandler_;
    //回调函数句柄//特别注释，epoll监听的可读事件就是连接事件
	CallBack WriteHandler_;
	CallBack CloseHandler_;
	CallBack ErroeHandler_;
	//CallBack MessageCallback_;//TCP连接的处理函数
	bool flag_;
    //为1时表示channel已经被添加到epoll中，反之则没有

};

typedef std::shared_ptr<Channel> SP_Channel;
