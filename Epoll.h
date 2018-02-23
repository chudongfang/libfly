
#ifndef _EPOLL_H__
#define _EPOLL_H__

#include <stdint.h>
#include <stdlib.h>
#include <iostream>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "Log.h"
#include "Time_heap.h"
#include "Callback.h"
#include "routine.h"
namespace libfly 
{





//向前声明
class Timer_Epolls;



//针对于每个epoll_event
class Timer_Epoll : public heap_timer
{
public:
    Timer_Epoll()
    { }
    ~Timer_Epoll()
    { }
public:
    
    //存储对应的fd
    int selfFd_; 
    
    EpollCallback epollCallback_;

    //指向Timer_Epolls
    Timer_Epolls * timer_Epolls_;
    
    //epoll返回结果
    struct epoll_event env_;

};




//epoll定时器
class Timer_Epolls : public heap_timer
{

public:
    Timer_Epolls(TimerCallback timerCallback,  void* arg, 
                 int delay, unsigned long long evsNum, int epfd, 
                 struct epoll_event * revents);

    Timer_Epolls(){};
    ~Timer_Epolls();
public:
    

    int epfd_;   //epollfd
    struct epoll_event *revents_;
    unsigned long long evsNum_;
    int RaiseNum_;
    bool isOutTime_;
    Timer_Epoll * timer_Epoll_ ;

};





//用于存储epoll_wait的结果
class epoll_res
{
public:
    
    epoll_res(int size)
    :  size_(size)
    {
        events_ =  (struct epoll_event*)calloc( 1,size_ * sizeof( struct epoll_event ) );
    }
    ~epoll_res()
    {
        delete events_;
    }

public:
	int size_;
	struct epoll_event *events_;
};






//协程中的Epoll对象
class Epoll
{

public:
    Epoll()
    {
        epollFd_ = epoll_create(epollSize_);
        result_ = new epoll_res(epollSize_);
    }

    ~Epoll()
    {
        delete result_;
    }
    
    int addEpoll( struct epoll_event* evs,unsigned long long evNum, 
                  struct epoll_event * revents , int  timeout,
                  unsigned long long maxNum = 1024 * 10);
    
public:
    int epollFd_;
    static const int epollSize_ = 1024 * 10;
    epoll_res * result_;
};





/*
int 	co_epoll_wait( int epfd,struct co_epoll_res *events,int maxevents,int timeout );
int 	co_epoll_ctl( int epfd,int op,int fd,struct epoll_event * );
int 	co_epoll_create( int size );
struct 	co_epoll_res *co_epoll_res_alloc( int n );
void 	co_epoll_res_free( struct co_epoll_res * );
*/



}
#endif
