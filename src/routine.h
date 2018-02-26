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
#include"routine.h"
#include"coctx.h"
#include"Time_heap.h"

namespace libfly
{

//向前声明
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
    void resume();   //运行当前协程
    void yield();    //退出当前协程
public:
    
    RoutineEnv * env_;                //当前协程环境
    Routine_Function pfn_;            //协程块函数
    void* arg_;                       //协程块函数对应的参数
    coctx_t ctx_;                     //用来保存CPU上下文

    char Start_;                      //协程是否运行
    char End_;                        //协程是否结束
    char IsMainRoutine_;              //是否是主协程

    StackMemry* stack_memry_;         //协程运行栈内存
    
    char* stack_sp_;                  //顶指针
    unsigned int save_size_;          //buff大小
    char* save_buffer_;               //buff

    unsigned long long getId() const  //获取协程ID
    {  return Id_; }

    void setId(unsigned long long Id) //设置协程ID
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
	Routine* CallStack_[ 128 ];       //保存调用链
	int CallStackSize_;               //栈指针
	//for copy stack log lastco and nextco 
	Routine* pending_;
    Routine* occupy_;
    
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
