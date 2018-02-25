# libfly
  libfly协程库，采用C++作为编程语言，封装了协程类，并提供了协程基本操作和调度功能
  
  
#### 一个简单的生产者-消费者的协程实现
```
using namespace libfly;

Conditional_variable cond;
int product;

void *timerCallback(Routine * routine)
{
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

int main()
{
    
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
```
