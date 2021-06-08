

#include "CountDownLatch.h"
#include "MutexLock.h"

CountDownLatch :: CountDownLatch(int count)
:count_(count),mutex_(),condition_(mutex_){}

CountDownLatch::~CountDownLatch(){}


void CountDownLatch::wait(){
	MutexLockGuard lockguard(mutex_);
	while(count_ > 0) condition_.wait();

}

void CountDownLatch::countdown(){
	MutexLockGuard lockguard(mutex_);
	count_--;
	if(count_ == 0){
		condition_.notifyAll();
	}

}