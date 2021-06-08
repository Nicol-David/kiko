
#pragma once
#include "Epoll.h"
#include "Channel.h"
#include <stdio.h>
#include <memory>
#include <functional>
#include "MutexLock.h"
#include "CurrentThread.h"
//using namespace lidan;

class Channel;
class Epoll;
typedef std::shared_ptr<Channel> SP_Channel;

class EventLoop{
public:
    typedef std::function<void()> Functor;
    EventLoop();
    ~EventLoop();
    bool isInLoopThread() const{
        return threadId_==CurrentThread::tid();
    }
    void assertInLoopThread(){
        assert(isInLoopThread());
    }
    void updatechannel(Channel* newchannel);
    //更新channel在epoll中的状态；
    void removechannel(Channel* newchannel);
    //将channel从epoll中移除
    void doPendingFunctors();
    void loop();
    //eventloop中的循环函数；
    int GetEooplFd();
    //得到epoll的文件描述符；
    void wakeup();
    //epoll从epoll_wait()中唤醒
    void handleRead();
    //pwakeupChannel_的可读事件执行函数
    void quit();

    void runInLoop(Functor&& cb);
    //可跨线程执行的函数，Functor函数可以被直接执行，也可添加入ququeInLoop中，在适当的时候执行
    void queueInLoop(Functor&& cb);

    friend class Epoll;

private:

    std::unique_ptr<Epoll> epoll_;
    int wakeupFd_;
    //用wakeupfd来唤醒epoll_wait函数，其顺序应该在pwakeupChannel_之前；
    SP_Channel pwakeupChannel_;
    std::vector<Functor> pendingFunctors_;
    //用于处理IO任务以外的计算任务，从这里开始
    mutable MutexLock mutex_;
    //为了保护pendingFunctors_而设置的线程号
    const pid_t threadId_;
    //EventLoop所属的线程号，在类被构造时就进行初始化；
    bool quit_;
    //为0时循环，反之则退出；
    bool eventHandling_;
    //为0时，IO线程不再执行计算任务，为1时表示正在执行计算任务；
    bool looping_;
    //为0时表示IO线程不再执行计算任务，反之则执行
    std::vector<Channel*> ChannelList_;
    //epoll中返回的需要处理的IO事件的channel都存储在此处；
    bool callingPendingFunctors_;


};