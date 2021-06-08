#pragma once
#include <pthread.h>
#include <functional>
#include <string>
#include "CountDownLatch.h"
#include "noncopyable.h"

class Thread::noncopyable{
public:
    typedef std::function<void()> ThreadFunc;
    explicit Thread(const ThreadFunc& func,const std::string& n=std::string());
    ~Thread();
    void start();
    int join();
    bool started() const {return started_;}
    pid_t tid() const {return tid_;}
    const std::string& name() const {return name_;}

private:
    void setDefaultName();
    bool started_;
    //子线程是否开启
    bool joined_;
    //子线程是否归并
    pthread_t pthreadId_;
    //子线程的线程号，进程内唯一
    pid_t tid_;
    //子线程的线程号，全局唯一
    //pthread_t与pid_t都可获得线程号，但是含义不同，内容不同，但都指向同一个线程
    ThreadFunc func_;
    //子线程的线程入口；
    std::string name_;
    //子线程名字
    CountDownLatch latch_;
    //用来告知父线程子线程已经开始运行了
}