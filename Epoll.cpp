#include "Epoll.h"

using namespace std;

Epoll::Epoll()
:epollfd_(epoll_create1(EPOLL_CLOEXEC)){
    //EPOLL_CLOEXEC：这是这个参数唯一的有效值，如果这个参数设置为这个。
    //那么当进程替换映像的时候会关闭这个文件描述符，这样新的映像中就无法对这个文件描述符操作，适用于多进程编程+映像替换的环境里
    events_.resize(FDSIZE);
}

Epoll::~Epoll(){}

void Epoll::epoll_update(Channel* new_channel){
    if(new_channnel->getflag()){
        //flag为1时表示已经添加event已经添加到epoll中
        epoll_mod(new_channel);
    }
    else {
        epoll_add(new_channel);
        new_channel->setflag(true);
        //在epoll中添加event的信息了，所以要将flag标志位置为1；
    }
}

void Epoll::epoll_add(Channel* new_channel){
    struct epoll_event ev;
    //设置epoll_event结构体，程式化的内容了
    ev.data.fd=new_channel->getfd();
    //试试对应理解一下sockfd;
    ev.events=new_channel->getEvents();
    epoll_ctl(epollfd_,EPOLL_CTL_ADD,new_channel->getfd(),&ev);
    fcntl(new_channel->getfd(),F_SETFL,fcntl(new_channel->getfd(),F_GETFD,0) | O_NONBLOCK);
    //更改文件描述符属性，希望将套接字改为非阻塞状态
    ChannelMap[new_channel->getfd()]=new_channel;
    //在哈希表中增加文件描述符对应的事件
    return;
}

void Epoll::epoll_mod(Channel* old_channel){
    struct epoll_struct event;
    event.data.fd=old_channel->getfd();
    event.events=old_channel->getEvents();
    epoll_ctl(epollfd_,EPOLL_CTL_MOD,old_channel->getfd(),&event);
    return;
}

void Epoll::epoll_del(Channel* the_channel){
    struct epoll_event event;
    event.data.fd=the_channel->getfd();
    event.events=the_channel->getEvents();
    epoll_ctl(epollfd_,EPOLL_CTL_DEL,the_channel->getfd(),&event);
    ChannelMap.erase(the_channel->getfd());
}

int Epoll::GetFd(){
    return epollfd_;
}

void Epoll::poll(std::vector<Channel*>& ChannelList){
    events_.clear();
    events_.resize(FDSIZE);
    //对应的epoll_event结构体
    int epoll_events_count=epoll_wait(epollfd_,&*events_.begin(),FDSIZE,EPOLLWAIT_TIME);
    //第二个参数，保存发生事件的文件描述符集合的结构体地址
    if(epoll_events_count==0){

    }
    else{
        printf("request number:%d",epoll_events_count);
    }
    assert(epoll_events_count>=0);
    for(int i=0;i<epoll_events_count;i++){
        if(ChannelMap.find(event_[i].data.fd!=ChannelMap.end())){
            ChannelMap[events_[i].data.fd]->SetRevent(events_[i].events);
            //该事件已经经过注册，将其标记为活动事件
            ChannelList.push_back(ChannelMap[events_[i].data.fd]);
        }
    }
}