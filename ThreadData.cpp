
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
#include "ThreadData.h"



//namespace CurrentThread;



ThreadData::ThreadData(const ThreadFunc& func, const std::string& name, pid_t* tid,CountDownLatch* latch)
				: func_(func), name_(name), tid_(tid), latch_(latch) {}

void ThreadData::runInThread() {
	*tid_ = CurrentThread::tid();//这里将子线程的线程号，传递给了父线程
	tid_ = NULL;
	latch_->countdown();//通知父线程子线程已经开始运行
	latch_ = NULL;     //这两个指针的实体都在父线程中，而此函数是在子线程中运行的，因此将两个指针设为空

	CurrentThread::t_threadName = name_.empty() ? "Thread" : name_.c_str();
	prctl(PR_SET_NAME, CurrentThread::t_threadName);//设置线程名字

	func_();//线程执行函数
	CurrentThread::t_threadName = "finished";
}


