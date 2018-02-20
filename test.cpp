
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
using namespace libfly;
void *timerCallback(Routine * routine)
{
    std::cout<<"I'm timerCallback Function. I will resume this routine!\n"<<std::endl;
    routine->resume();
}

void * Run_In_Routine_1(void *arg)
{
    while(1)
    {
        std::cout<<"I'm The Function In Routine 1"<<std::endl;
        Poller(timerCallback , get_curr_routine(),1);
    }
}


int x= 2;
int main()
{
    std::vector<Routine*> RoutineArr; 
    RoutineArr.push_back(new Routine(get_curr_thread_env(),NULL,Run_In_Routine_1,NULL) );	
    for(int i=0;i<1;i++)
    {
        RoutineArr[i]->resume();
    }
    std::cout<<"I'm Main routine"<<std::endl; 
    EventLoop eventloop(get_curr_thread_env()->time_heap_,NULL,NULL);
    eventloop.loop();
    return 0;
}





