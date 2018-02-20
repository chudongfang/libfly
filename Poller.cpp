/*************************************************************************
	> File Name: Poller.cpp
	> Author: 
	> Mail: 
	> Created Time: 2018年02月19日 星期一 21时37分27秒
 ************************************************************************/
#include"Poller.h"

namespace libfly
{

Poller::Poller( TimerCallback  timerCallback , void * arg , int delay)
  : time_heap_(get_curr_thread_env()->time_heap_),
    timerCallback_(timerCallback),
    arg_(arg),
    delay_(delay)

{
    
  //  assert(timerCallbac);
    if(delay_ < 0)
    {
        delay_ = 1e9;
    }
    time_heap_->add_timer(timerCallback_, arg_, delay_);
    
    get_curr_routine()->yield();
}

Poller::~Poller()
{

}



}


