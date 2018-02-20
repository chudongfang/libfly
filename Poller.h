/*************************************************************************
	> File Name: Poller.h
	> Author: 
	> Mail: 
	> Created Time: 2018年02月17日 星期六 00时26分30秒
 ************************************************************************/

#ifndef _POLLER_H
#define _POLLER_H
#include"Time_heap.h"
#include"Callback.h"
#include"assert.h"
#include"routine.h"
namespace libfly
{


class Poller
{
public:
    Poller(TimerCallback  timerCallback , void * arg , int delay);
    ~Poller();

    
public:    
    Time_heap * time_heap_;
    TimerCallback timerCallback_;
    void * arg_;
    int delay_;

};































}















#endif
