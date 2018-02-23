/*************************************************************************
	> File Name: routine.h
	> Author: 
	> Mail: 
	> Created Time: 2018年02月10日 星期六 23时40分57秒
 ************************************************************************/

#ifndef _ROUTINE_H
#define _ROUTINE_H
#include<cstdio>
#include<iostream>
#include<pthread.h>
#include<string>
#include<functional>
#include<Callback.h>
#include"coctx.h"
#include"Time_heap.h"
#include"Epoll.h"

namespace libfly
{

class RoutineEnv;
class Spec;
class StackMemry;
class RoutineAttr;


//协程块
class Routine 
{
public:
    //FIXME RoutineAttr use the *
    Routine(RoutineEnv * env,const RoutineAttr* attr ,void*(*pfn)(void*),void *arg );
    ~Routine();
    void resume();
    void yield();
    void release();



public:
    
    RoutineEnv * env_;                //当前协程环境
    Routine_Function pfn_;            //协程块函数
    void* arg_;                       //协程块函数对应的参数
    coctx_t ctx_;                     //用来保存CPU上下文

    char Start_;                      //协程是否运行
    char End_;
    char IsMainRoutine_;              //是否是主协程

    void * pvEnv_;
    
    StackMemry* stack_memry_;         //协程运行栈内存
    
    char* stack_sp_;
    unsigned int save_size_;
    char* save_buffer_;

    unsigned long long getId() const
    {  return Id_; }

    void setId(unsigned long long Id)
    { Id_ = Id; }

private:
    unsigned long long Id_;           //协程ID
};



//协程栈
class StackMemry
{
public:
    StackMemry(int stack_size);
public:
    Routine* occupy_co_;     //当前协程指针
	int      stack_size_;    //栈大小
	char*    stack_bp_;      //stack_buffer + stack_size
	char*    stack_buffer_;  //栈空间
};



class RoutineAttr//协程的参数
{
public:
    RoutineAttr()
    {
        stack_size_ = 128 * 1024;
    }
public:
    int stack_size_; //栈大小
};







class RoutineEnv
{
public:
    
    RoutineEnv();
    ~RoutineEnv();
public:
	Routine* CallStack_[ 128 ];       //保存调用链
	int CallStackSize_;               //栈指针
	//for copy stack log lastco and nextco 
	Routine* pending_;
    Routine* occupy_;
    
    Epoll * epoll_;
    //时间堆定时器
    Time_heap* time_heap_;
};


//------------协程调度-------------------------------


class EventLoop
{
public:
    EventLoop(Time_heap* time_heap, EventLoop_Function runInLoopFunction , void *arg );
    ~EventLoop();
    void loop();
    void runInLoop();


public:
    EventLoop_Function runInLoopFunction_; 
    void *arg_;
    
    //定时器指针
    Time_heap* time_heap_;
};


//--------------------------

RoutineEnv* get_curr_thread_env();
Routine * get_curr_routine();




}


#endif
