
#include "EventLoop.h"
#include "Epoll.h"
#include "Socket.h"
#include <sys/eventfd.h>

int createEventfd(){
    int evtfd=eventfd(0,EFD_NONBLOCK | EFD_CLOEXEC);
    //生成一个非阻塞的可读事件，这个事件是一个计数器；
    if(evtfd<0){
        abort();
    }
    return evtfd;
}

EventLoop::EventLoop()
:epoll_(new Epoll()),
threadId_(CurrentThread::tid()),
wakeupFd_(createEventfd()),
quit_(0),
eventHandling_(0),
mutex_(),
looping_(0),
pwakeupChannel_(new Channel(this,wakeupFd_,EPOLLIN | EPOLLET)){
    pwakeupChannel_->SetReadHandl(std::bind(&EventLoop::handleRead,this));
}

EventLoop::~EventLoop(){}

void EventLoop::updatechannel(Channel* newchannel){
    epoll_->epoll_update(newchannel);
}

void EventLoop::removechannel(Channel* thechannel){
    epoll_->epoll_del(thechannel);
    return;
}

int EventLoop::GetEpoolFd(){
    return epoll_->GetFd();
}

void EventLoop::loop(){
    assert(!looping_);
    assert(isInLoopThread());
    //loop函数是不可跨线程调用

    looping_=true;
    quit_=false;
    while(!quit_){
        ChannelList_.clear();
        epoll_->poll(ChannelList_);
        //调用epoll的类内函数，将活动事件加入ChannelList_，这里可以在epoll函数中找到答案
        eventHandling_=true;
        for(int i=0;i<ChannelList_.size();i++){
            ChannelList_[i]->EventHandl();
            //处理ChannelList_中的每个活动事件
            printf("in loop\n");
        }
        evenrHandling_=false;
        doPendingFunctors();
        //处理计算任务
    }
    looping_=false;
}

void EventLoop::doPendingFunctors(){//目的就是处理线程计算任务
    std::vector<Functor> functors;
    callingPendingFunctors_=true;
    {
        MutexLockGuard lock(mutex_);
        functors.swap(pendingFunctors_);
        //pendingFunctors_中的内容被复制到functors,并将pendingFunctors_中的内容清空
    }
    for(size_t i=0;i<functors.size();i++)
        functors[i]();
    //由于functors可能再调用queueInLoop（cb），这是queueInLoop就必须wakeuo（），否则新加的cb就不能被及时调用
    callingPendingFunctors_=false;
}

void EventLoop::wakeup()
{
    unit64_t one=1;
    ssize_t n=socket::read(wakeFd_,&one,sizeof one);

}

void EventLoop::handleRead(){
    unit64_t one=1;
    ssize_t n=socket::read(wakeupFd_,&one,sizeof one)
}

void EventLoop::quit()//跨线程函数
{
    quit_=true;
    if(!isInLoopThread())
        wakeup();
        //唤醒epoll,使得loop更加快速的退出
}

//函数目的：轻易地在线程间调配任务
void EventLoop::runInLoop(Functor&&　cb){//该函数的内容是在它的IO线程内执行某个用户任务的回调
//如果用户在当前IO线程中调用runInLoop函数，则回调函数会同步进行
    if(isInLoopThread())
        cb();
    else{
        //如果用户在其他线程调用runInLoop函数，则cb（）会被加入队列，IO线程被唤醒来调用这个Functor
        queueInLoop(std::move(cb));
    }
}

void EventLoop::queueInLoop(Functor&& cb){ //将cb放入队列，在必要时唤醒IO线程
    {
        MutexLockGuard lock(mutex_);
        pendingFunctors_.emplace_back(std::move(cb));
        //pendingFunctors_在这里将要处理的任务压入内容，pendingFunctors_将成为一个临界区
    }
    if(!isInLoopThread() || callingPendingFunctors_)
        wakeup();
    //对于“必要时”的理解：1.调用queueInLoop()的不是IO线程，唤醒是必须的；
    //2.如果在IO线程调用queueInLoop(),而此时正在调用pending functor。
    //只有在IO线程的事件回调中调用queueInLoop()才无需wakeup();
}