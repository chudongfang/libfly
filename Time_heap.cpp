
#include "Time_heap.h"
namespace libfly
{




//定时事件构造函数
heap_timer::heap_timer( TimerCallback timerCallback,  void* arg,  int delay)
 :  timerCallback_(timerCallback),
    arg_(arg)
{
    expire = time(NULL) + delay;
}
    


//-----------------------------

Time_heap::Time_heap(int cap)
  : capacity(cap),
    cur_size(0)
{
    array = new heap_timer*[capacity];
    if(!array)
    {
        //FIXME throw 
    }
    for(int i = 0 ;i < capacity; i++)
    {
        array[i] = NULL;
    }
}


Time_heap::~Time_heap()
{
    for(int i = 0;i < cur_size; i++)
    {
        delete array[i];
    }
    delete [] array;
}




//添加定时器
void Time_heap::add_timer(heap_timer* timer)
{
    if(!timer)
    {
        return;
    }
    
    if(cur_size >= capacity)
    {
        resize();
    }

    int hole = cur_size++;
    int parent = 0;

    for(; hole>0 ;hole = parent)
    {
        parent = (hole-1)/2;
        if(array[parent] -> expire <= timer->expire)
        {
            break;
        }
        array[hole] = array[parent];
    }
    array[hole] = timer;

}

//添加定时器
void Time_heap::add_timer( TimerCallback timerCallback,  void* arg,  int delay)
{


    heap_timer * timer = new heap_timer(timerCallback , arg , delay);
    if(!timer)
    {
        return;
    }
    
    assert(timer->timerCallback_);
    if(cur_size >= capacity)
    {
        resize();
    }

    int hole = cur_size++;
    // std::cout<<"hole :"<<parent<<std::endl;
    int parent = 0;
    for(; hole>0 ;hole = parent)
    {
        parent = (hole-1)/2;
        if(array[parent] -> expire <= timer->expire)
        {
            break;
        }
        array[hole] = array[parent];
    }
    array[hole] = timer;

}


//删除一个指定的定时器
void Time_heap::del_timer(heap_timer* timer)
{
    if(!timer)
    {
        return;
    }

    timer->timerCallback_ = NULL;
}


//获取头结点定时器
heap_timer* Time_heap::top() const 
{
    if(empty())
    {
        return NULL;
    }
    return array[0];
}


//删除头节点
void Time_heap::pop_timer()
{
    if(empty())
    {
        return ;
    }
    if(array[0])
    {
        delete array[0];
        array[0] = array[--cur_size];
        push_down(0);
    }
}
       

//心跳函数,执行到时间的定时器
void Time_heap::runOutTimeEvent()
{
    //如果没有定时任务
    //则直接退出
    
    //DEBUG
    //std::cout<<cur_size<<std::endl;
    if(cur_size == 0)
    {
        return ;
    }

    heap_timer* tmp = array[0];
    time_t cur = time(NULL);
    while(!empty())
    {
        if(!tmp)
        {
            break;
        }
        //std::cout<<tmp->expire<<" "<<cur<<std::endl;
        
        if(tmp->expire > cur)
        {
            break;
        }

        if(array[0]->timerCallback_)
        {
           // std::cout<<"Time_heap:: Run timerCallback_!"<<std::endl;
            array[0]->timerCallback_((Routine*) (array[0]->arg_));
        }
        pop_timer();
        tmp = array[0];

    }
}


void Time_heap::push_down(int hole)
{
    heap_timer* temp = array[hole];
    int child = 0;
    for( ; ( hole*2+1 ) <= (cur_size-1) ; hole = child)
    {
        child = hole * 2 + 1;
        if( ( child < (cur_size - 1) ) && ( array[child + 1]->expire < array[child]->expire ) )
        {
            child++;
        }

        if(array[child]->expire < temp->expire)
        {
            array[hole] = array[child];
        }
        else 
        {
            break;
        }
    }
    array[hole] = temp;
}


//将数组扩大一倍
void Time_heap::resize()
{
    heap_timer** temp = new heap_timer*[2*capacity];
    for(int i = 0; i < 2*capacity; i++)
    {
        temp[i] = NULL;
    }
    if(!temp)
    {
        //FIXME use throw
    }
    capacity *= 2;
    for(int i = 0; i < cur_size; ++i)
    {
        temp[i] = array[i];
    }
    delete [] array;
    array = temp;
}














}

