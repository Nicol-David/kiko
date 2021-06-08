//作者：shao chuang
//QQ群：1020789851
//时间：2020.04.30

#pragma once
#include <pthread.h>
#include "noncopyable.h"

class MutexLock: noncopyable{
	public:
		MutexLock(){pthread_mutex_init(&mutex_, NULL);}
		~MutexLock(){
			pthread_mutex_lock(&mutex_);
			pthread_mutex_destroy(&mutex_);
		}
		void lock(){pthread_mutex_lock(&mutex_);}
		void unlock(){pthread_mutex_unlock(&mutex_);}
		pthread_mutex_t *get() { return &mutex_; }
	private:
		pthread_mutex_t mutex_;



};



//MutexLockGuard的作用是用其自身的构造和析构来完成MutexLock类的lock和unlock方法；
//这样就只用来管理MutexLockGuard的生存周期，就能保证lock和unlock方法的成对使用
class MutexLockGuard: noncopyable{
	public:
		explicit MutexLockGuard(MutexLock& mutexlock):mutexlock_(mutexlock){mutexlock_.lock();}
		~MutexLockGuard(){mutexlock_.unlock();}

	private:
		MutexLock& mutexlock_;



};

