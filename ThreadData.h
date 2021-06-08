
//作者：shao chuang
//QQ群：1020789851
//时间：2020.05.09

#pragma once
#include <functional>
#include <pthread.h>
#include <sys/prctl.h>
#include <assert.h>
#include <errno.h>
#include <linux/unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <memory>
#include <string>

#include "CountDownLatch.h"
#include "CurrentThread.h"



//namespace CurrentThread;


//每一个子线程都会拥有一个ThreadData，ThreadData也维护了，子线程的入口函数

class ThreadData{
	public:
		typedef std::function<void ()> ThreadFunc;
	public:
		ThreadData(const ThreadFunc& func, const std::string& name, pid_t* tid,CountDownLatch* latch);

		 void runInThread();

	public:
		
		ThreadFunc func_;
		//线程的实际函数入口
		std::string name_;
		//线程名
		pid_t* tid_;
		//全局唯一的线程号，这只是一个指针其实体在父线程中，用指针的方式将子线程的线程号，传递给了父线程
  		CountDownLatch* latch_;
		//此门闩类是为了通知父线程，子线程已经开始运行，全局唯一的线程号，这只是一个指针其实体在父线程中，

};

