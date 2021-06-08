#include "EventLoopThreadPool.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseLoop,int numThreads)
:baseLoop_(baseLoop),
started_(false),
numThreads_(numThreads),
next_(0){
    if(numThreads_<=0){
        abort();
    }
}

void EventLoopThreadPool::start(){
    baseLoop_->assertInLoopThread();
    started_=true;
    for(int i=0;i<numThreads_;i++){
        //创建线程池，将创建的线程放入loops_中
        std::shared_ptr<EventLoopThread> t(new EventLoopThread());
        //创建thread，且在每个thread中创建EventLoop
        threads_.push_back(t);
        loops_.push_back(t->startLoop());
    }
}

EventLoop *EventLoopThreadPool::getNextLoop(){
    baseLoop_->assertInLoopThread();
    assert(started_);
    EventLoop* loop=baseLoop_;
    //如果线程池为空，直接使用主EventLoop
    if(!loops_.empty()){
        loop=loops_[next_];
        next_=(next_+1)%numThreads_;
    }
    return loop;
}