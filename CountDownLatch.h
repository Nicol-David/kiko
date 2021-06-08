
//作者：shao chuang
//QQ群：1020789851
//时间：2020.04.30



#pragma once
#include "Condition.h"
#include "MutexLock.h"
#include "noncopyable.h"


//此类相当于一个带计数器的条件变量
class CountDownLatch:noncopyable{
	public:
		CountDownLatch(int count);
		~CountDownLatch();
		void wait();
		void countdown();
	private:
		MutexLock mutex_;
		Condition condition_;
		int count_;



};