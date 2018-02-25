//test for Conditional_variable


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <iostream>
#include "routine.h"
#include "routine.cpp"
#include <vector>
#include "Time_heap.h"
#include "Time_heap.cpp"
#include "Poller.h"
#include "Poller.cpp"
#include "Conditional_variable.cpp"
#include "Conditional_variable.h"
#include "Log.h"
#include "Log.cpp"

using namespace libfly;



Conditional_variable cond;
int product;


void *timerCallback(Routine * routine)
{
    //std::cout<<"I'm timerCallback Function. I will resume this routine!\n"<<std::endl;
    routine->resume();
}

void * Producer(void *arg)
{
    while(1)
    {
        product ++;
        product ++;
        product ++;
        std::cout<<"Producer  product::"<<product<<std::endl;
        cond.notify_one();
        
        //返回主协程
        Poller(timerCallback , get_curr_routine(),1); 
        
    }
}

void * Consumer(void *arg)
{
    while(1)
    {

        if(product == 0)
            cond.wait();
        product--;
        std::cout<<"Consumer  product::"<<product<<std::endl;
    }
}

Log mylog("libfly");
int main()
{
    
    LOG_TRACE(mylog,"Yes!");
    LOG_DEBUG(mylog,"debug");
    std::vector<Routine*> RoutineArr; 
    RoutineArr.push_back(new Routine(get_curr_thread_env(),NULL,Consumer,NULL) );	
    RoutineArr.push_back(new Routine(get_curr_thread_env(),NULL,Producer,NULL) );	
    for(int i=0;i<2;i++)
    {
        RoutineArr[i]->resume();
    }
    std::cout<<"I'm Main routine"<<std::endl; 
    EventLoop eventloop(get_curr_thread_env()->time_heap_,NULL,NULL);
    eventloop.loop();
    return 0;
}





