//作者：shao chuang
//QQ群：1020789851
//时间：2020.04.30


#pragma once

#include "MutexLock.h"
#include "noncopyable.h"
#include <pthread.h>


class Condition:noncopyable{
	public:
		explicit Condition(MutexLock& mutex):mutex_(mutex){pthread_cond_init(&cond_, NULL);}
		~Condition() { pthread_cond_destroy(&cond_); }
		void wait(){pthread_cond_wait(&cond_, mutex_.get());}
		void notify(){pthread_cond_signal(&cond_);}
		void notifyAll(){pthread_cond_broadcast(&cond_); }

	private:
		MutexLock& mutex_;
		pthread_cond_t cond_;






};

