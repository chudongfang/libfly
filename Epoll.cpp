
#include "Epoll.h"

#include"Log.h"

namespace libfly
{



static void * timerCallback( Routine * routine )
{
    routine->resume();
}



//用于Epoll产生事件时唤醒Timer_Epolls定时器
static void * epollCallback(void * arg)
{
    LOG("libfly");
    
    Timer_Epoll * timer_Epoll =  (Timer_Epoll*) arg ;
    Timer_Epolls *timer = timer_Epoll -> timer_Epolls_;
   
    //赋值revents
    timer->revents_[timer->RaiseNum_] = timer_Epoll->env_; 
    //赋值对应的fd
    timer->revents_[timer->RaiseNum_].data.fd = timer_Epoll->selfFd_;
    timer->RaiseNum_++;
    
    LOG_DEBUG("Epoll come !");
    if( !timer->isOutTime_ )
    {
        timer->isOutTime_ = 1;
        
        Timer_Epolls *timerNew  = new Timer_Epolls(); 
        memcpy(timerNew ,timer ,sizeof (Timer_Epolls));
        timerNew->expire=time(NULL)+1;//FIXME +1 ?
        get_curr_thread_env()->time_heap_->add_timer(timerNew);
        
        std::cout<<"Timer_Epolls will outtime!"<<std::endl;

        get_curr_thread_env()->time_heap_->del_timer(timer);
    }
}






Timer_Epolls::Timer_Epolls(TimerCallback timerCallback, void* arg, 
                          int delay, unsigned long long evsNum, int epfd,
                          struct epoll_event* revents)
  : heap_timer(timerCallback, arg, delay),
    epfd_(epfd),
    evsNum_(evsNum),
    RaiseNum_(0),
    revents_(revents)
{
    timer_Epoll_ =  new Timer_Epoll [evsNum_]  ;
}


Timer_Epolls::~Timer_Epolls()
{
    delete timer_Epoll_;
}







int Epoll::addEpoll( struct epoll_event* evs, unsigned long long evNum, 
                    struct epoll_event * revents,int  timeout,
                    unsigned long long maxNum)
{

    Timer_Epolls * arg = new Timer_Epolls(timerCallback, get_curr_routine(), timeout, evNum, epollFd_, revents);
    
    

    for(unsigned long long i = 0 ; i < evNum ; i++)
    {
        //初始化timer_epoll信息
        arg->timer_Epoll_[i].timer_Epolls_  = arg;
        arg->timer_Epoll_[i].epollCallback_ = epollCallback;
        arg->timer_Epoll_[i].selfFd_        = evs[i].data.fd; 
        arg->timer_Epoll_[i].env_.events    = evs[i].events; 
        arg->timer_Epoll_[i].env_.data.ptr  = arg->timer_Epoll_+i;

        if(arg->timer_Epoll_[i].selfFd_ < 0) continue;

        
        int ret = epoll_ctl( epollFd_, EPOLL_CTL_ADD, arg->timer_Epoll_[i].selfFd_,
                             &arg->timer_Epoll_[i].env_);
        assert(ret == 0);
         
    }
    

    
    //把Timer_Epolls定时事件加入定时器
    get_curr_thread_env()->time_heap_->add_timer(arg);
    
    //退出当前协程
    get_curr_routine()->yield(); 
    
    //------comeback to this routine---------
    
    //删除加入的socket 
    for(unsigned long long i = 0 ; i < evNum ; i++)
    {
        if(evs->data.fd < 0) continue;

        int ret = epoll_ctl( epollFd_, EPOLL_CTL_DEL, arg->timer_Epoll_[i].selfFd_, 
                            &arg->timer_Epoll_[i].env_ );
        assert(ret == 0);
    }
   


    return arg->RaiseNum_;
}



}
/*
int	co_epoll_wait( int epfd,struct co_epoll_res *events,int maxevents,int timeout )
{
	return epoll_wait( epfd,events->events,maxevents,timeout );
}
int	co_epoll_ctl( int epfd,int op,int fd,struct epoll_event * ev )
{
	return epoll_ctl( epfd,op,fd,ev );
}
int	co_epoll_create( int size )
{
	return epoll_create( size );
}

struct co_epoll_res *co_epoll_res_alloc( int n )
{
	struct co_epoll_res * ptr = 
		(struct co_epoll_res *)malloc( sizeof( struct co_epoll_res ) );

	ptr->size = n;
	ptr->events = (struct epoll_event*)calloc( 1,n * sizeof( struct epoll_event ) );

	return ptr;

}
void co_epoll_res_free( struct co_epoll_res * ptr )
{
	if( !ptr ) return;
	if( ptr->events ) free( ptr->events );
	free( ptr );
}


*/
