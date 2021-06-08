#pragma once

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/epoll.h>
#include <fcntl.h>

#include <vector>
#include <map>
#include <memory>

#include "Channel.h"

#define FDSIZE 1024
#define EPOLLWAIT_TIME 10


class Channel;
typedef std::shared_ptr<Channel> SP_Channel;

class Epoll{
public:
    Epoll();
    ~Epoll();
    void epoll_update(Channel* new_channel);
    void epoll_add(Channel* new_channel);
    void epoll_mod(Channel* old_channel);
    void epoll_del(Channle* the_channel);
    int GetFd();
    void poll(std::vector<Channel*>& ChannelList);

    int epollfd_;
    std::vector<epoll_event> events_;
    //给epoll_wait()使用的epoll_event类型的数组；
    std::map<int,Channel*> ChannelMap;
    //所有在epoll上注册的channel的集合；
}