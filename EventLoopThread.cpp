#include "EventLoopThread.h"

EventLoopThread::EventLoopThread()
    : loop_(NULL),
      //exiting_(false),
      thread_(std::bind(&EventLoopThread::threadFunc, this), "EventLoopThread"),
      //创建thread类
      mutex_(),
      cond_(mutex_) {}

EventLoopThread::~EventLoopThread() {
  //exiting_ = true;
  if (loop_ != NULL) {
      //如果子线程还是没有退出，需要先将子线程退出
    loop_->quit();
    thread_.join();
  }
}

EventLoop* EventLoopThread::startLoop() {
  assert(!thread_.started());
  thread_.start();
  {
    MutexLockGuard lock(mutex_);
    // 一直等到threadFun在子线程里真正跑起来，也就是EventLoop在子线程中建立了
    //此时loop_才有意义。
    while (loop_ == NULL) cond_.wait();
  }
  return loop_;
}

void EventLoopThread::threadFunc() {//在子线程中运行的函数
  EventLoop loop;

  {
    MutexLockGuard lock(mutex_);
    loop_ = &loop;
    cond_.notify();
  }

  loop.loop();
  // assert(exiting_);
  loop_ = NULL;
  
}