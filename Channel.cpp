#include "cchannel.h"

Channel::Channel(EventLoop *loop,int fd,int event)
:loop_(loop),
fd_(fd),
event_(event),
flag_(0){}

Channel::Channel(EventLoop *loop,int fd)
:loop_(loop),
fd_(fd),
event_(0),
flag_(0){}

Channel::~Channel(){
    close(fd_);
}

Channel::getfd(){
    return fd_;
}

Channel::getEvents(){
    return event_;
}

void Channel::WriteHandl(){
    if(WriteHandler_) //function非空
        WriteHandler_();
}

void Channel::ReadHandl(){
    if(ReadHandler_)
        ReadHandler_();
}

void Channel::CloseHandl(){
    if(CloseHandler_)
        CloseHandler_();
}

void Channel::ErrorHanl(){
    if(ErrorHandler_)
        ErrorHandler_();
}

void Channel::EventHandl(){ //channel类的核心部分
    if((revent_& EPOLLHUP) && !(revent_ & EPOLLIN))
        //对端异常断开
        return;

    if(revent_ & EPOLLERR){
        ErrorHandl();
        return;
    }

    if(revent_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)){
        //这三个都是可读事件，EPOLLIN(可读、已连接)，EPOLLPRI(外带数据),EPOLLRDHUP(对端正常断开)
        ReadHandl();
    }

    if(revent_ & EPOLLOUT)
        WriteHandl();

    updata();

}

void Channel::updata(){
    loop_->updatachannel(this);
    //在loop中新增channel事件，而在loop的updatachannel(this)函数中调用的是epoll_->epoll_update(newchannel)函数
}

void Channel::remove(){
    loop_->removechannel(this);
}

