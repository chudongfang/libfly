#ifndef _TIME_HEAP
#define _TIME_HEAP

#include<iostream>
#include<netinet/in.h>
#include<time.h>
#include"routine.h"
#include<assert.h>

namespace libfly
{

class Routine;


class heap_timer//定时器类
{
public:
    heap_timer( TimerCallback timerCallback,  void* arg,  int delay);
    

public:
    //定时器生效时间
    time_t expire;
    
    //定时器中的回调函数
    TimerCallback timerCallback_;
    
    //定时器中的参数
    void* arg_;
};


class Time_heap
{

public:
    Time_heap(int cap);
    ~Time_heap();
    
public:
    
    //添加定时器
    void add_timer(heap_timer* timer);
    
    void add_timer( TimerCallback timerCallback,  void* arg,  int delay);
    //删除一个指定的定时器
    void del_timer(heap_timer* timer);
    

    //获取头结点定时器
    heap_timer* top() const ;
    
    //删除头节点
    void pop_timer();
           
    //心跳函数,执行到时间的定时器
    void runOutTimeEvent();
    
        
    bool empty() const 
    { return cur_size == 0; }
    
    int getcur_size() const 
    { return cur_size; }
    
    int getcapacity() const
    { return capacity; }



private:
    void push_down(int hole);
   
    //将数组扩大一倍
    void resize();
    
    
private:
    heap_timer** array;
    int capacity;
    int cur_size;
    //FIXME fix the name  add '_' 
};








}

#endif
