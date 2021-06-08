#pragma once
#include "EventLoop.h"
#include "EventLoopThread.h"
#include <vector>

//EventLoopThreadPool中维护这一个线程池，每一个子线程中都维护这一个EventLoop
class EventLoopThreadPool{
public:
    EventLoopThreadPool(EventLoop* baseLoop, int numThreads);

    ~EventLoopThreadPool() {}
    void start();

    EventLoop* getNextLoop();


private:

    EventLoop* baseLoop_;
    //这是整个程序中的主eventloop的指针，其负责端口的监听
    bool started_;
    //0表示线程池没有开始运行，1表示线程池已经开始工作
    int numThreads_;
    //线程池的线程总数
    int next_;
    //线程池以轮叫的方式来分配负载，next表示下一个要被叫到的EventLoopThread的下标
    std::vector<std::shared_ptr<EventLoopThread>> threads_;
    //线程池
    std::vector<EventLoop*> loops_; 
    //与线程池相对应的EventLoop指针

};