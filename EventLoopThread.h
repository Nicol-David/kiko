#pragma once
#include "EventLoop.h"
#include "Condition.h"
#include "MutexLock.h"
#include "Thread.h"
#include "noncopyable.h"

//EventLoopThread类对于EventLoop类和Thread类进行一个融合
class EventLoopThread:noncopyable{
public:
    EventLoopThread();
    ~EventLoopThread();
    EventLoop* startLoop();

private:
    void threadFunc();
    //子线程入口函数
    EventLoop* loop_;
    //EventLoop的实例是在Thread维护的子线程中建立的
    //父线程中只有EventLoop的指针
    Thread thread_;
    MutexLock mutex_;
    Condition cond_;

}