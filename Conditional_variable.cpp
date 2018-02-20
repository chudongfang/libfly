/*************************************************************************
	> File Name: Conditional_variable.cpp
	> Author: 
	> Mail: 
	> Created Time: 2018年02月20日 星期二 16时19分48秒
 ************************************************************************/
#include"Conditional_variable.h"

namespace libfly
{

static void *Cond_Callback(Routine *routine)
{
    routine->resume();
}

Conditional_variable::Conditional_variable()
{
    while(!routines_.empty()) routines_.pop();
    size_ = 0;
}

Conditional_variable::~Conditional_variable()
{

}


//wait,等待条件变量被唤醒
//生成一个定时事件,并保持
//退出当期协程
void Conditional_variable::wait()
{
    Routine * curr_routine = get_curr_routine();
    heap_timer * cur_timer = new heap_timer(Cond_Callback , curr_routine , 1);     
    routines_.push(cur_timer);
    size_++;
    curr_routine->yield();
}

//唤醒一个等待该条件变量的协程
void Conditional_variable::notify_one()
{
    if(routines_.empty()) 
        return ;

    heap_timer *cur_timer = routines_.front();
    get_curr_thread_env()->time_heap_->add_timer(cur_timer);
    routines_.pop();
    size_--;
}

//唤醒所有的等待条件变量的协程
void Conditional_variable::notify_all()
{
    while(!routines_.empty())
    {
        heap_timer *cur_timer = routines_.front();
        get_curr_thread_env()->time_heap_->add_timer(cur_timer);
        routines_.pop();
    }
    size_ = 0;
}




}





