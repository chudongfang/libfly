# libfly
  libfly协程库，采用C++作为编程语言，封装了协程类，并提供了协程基本操作和调度功能
  
    编写此协程库的目的是为了让协程理解起来更容易，让协程编程更加简单!
    正逐步完善代码注释与文档，欢迎大家一起讨论交流～

#### libfly基本的架构图

![libfly1](https://github.com/chudongfang/java_practice/blob/master/picture/libfly.png)


---


## Analysis

[http://blog.csdn.net/chudongfang2015?viewmode=contents](http://blog.csdn.net/chudongfang2015?viewmode=contents)


## How to use



#### 协程的创建
```c++
void* Func(void *arg)
{
    int* x = (int*)arg;
    std::cout<<*x<<std::endl;
}
int x = 1;
Routine  * routine = new Routine(get_curr_thread_env(),NULL,Func,&x);

```

---


#### 协程的唤醒和退出

```c++
void* Func(void *arg)
{
    int* x = (int*)arg;
    std::cout<<*x<<std::endl;
}
int x = 1;
Routine  * routine = new Routine(get_curr_thread_env(),NULL,Func,&x);
routine->resume();
routine->yiled();
```

---

#### 一个简单的生产者-消费者的协程实现
```c++
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

