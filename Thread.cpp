 #include "Thread.h"
 #include <assert.h>
 #include "ThreadData.h"

 Thread::Thread(const ThreadFunc& func,const std::string& n)
 :started_(false),
 joined_(false),
 pthreadId_(0),
 tid_(0),
 func_(func),
 name_(n),
 latch_(1){
     setDefaultName();
 }

 Thread::~Thread(){
     //如果Thread类在被销毁的时候，thread类维护的子线程还没有归并
     //那么就将子线程设置为detach状态，在此状态下，如果子线程运行结束将会自行清理内存
     if(started_ && !joined_) pthread_detach(pthreadId_);
 }

 void Thread::setDefaultName(){
     if(name_.empty()){
         //当然，name的类型肯定是string了
         char buf[32];
         snprintf(buf,sizeof buf,"Thread");
         //snprintf位于stdio.h头文件中
         name_=buf;
     }
 }

 void Thread::start(){
     assert(!started_);
     started_=true;
     ThreadData* data=new ThreadData(func_,name_,&tid_,&latch_);
     if(pthread_create(&pthreadId_,NULL,&CurrentThread::startThread,data)){
         //成功时返回0，所以这里对应的是子线程创建失败的情况
         started_=false;
         delete data;
     }
     else{
         latch_.wait();
         //成功时在此处开始阻塞，直至子线程开始
         assert(tid_>0);
     }
 }

 int Thread::join(){
     assert(started_);
     assert(!joined_);
     joined_=true;
     return pthread_join(pthreadId_,NULL);//开始阻塞，直到线程结束；
 }