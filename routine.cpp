/*************************************************************************
	> File Name: routine.cc
	> Author: 
	> Mail: 
	> Created Time: 2018年02月11日 星期日 00时56分37秒
 ************************************************************************/

#include<cstring>
#include<cstdlib>
#include<cstdio>
#include<string>
#include<map>
#include<poll.h>
#include<sys/time.h>
#include<errno.h>
#include<assert.h>
#include<fcntl.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/syscall.h>
#include<unistd.h>
#include<limits.h>
#include<memory>

#include"Callback.h"
#include"routine.h"
//#include"routine_inner.h"
#include<functional>
namespace libfly
{
extern "C"
{
    extern void coctx_swap( coctx_t *,coctx_t* ) asm("coctx_swap");
};

class Time_heap;

//-------debug---------
void dbg_flag(int line)
{
    std::cout<<"I'm Runing on "<< line <<std::endl;
}
void dbg_print(char * str)
{
    std::cout<< str<<std::endl;
}


//----------------Function Definition-----------------------------
static int RoutineFunc( Routine* routine,void * );
void yield_env(RoutineEnv* env_);
void yield_current_thread();
void co_swap(Routine* Current_Routine, Routine* Pending_Routine);
void init_curr_thread_env();
RoutineEnv* get_curr_thread_env();
Routine * get_curr_routine();


//---------------------toolsfunction------------------------------
//获取时间

//得到当前ID
static pid_t GetPid()
{
    static __thread pid_t pid = 0;
    static __thread pid_t tid = 0;
    if(!pid||!tid||pid != getpid())
    {
        tid = syscall(__NR_gettid);
    }
    return tid;
}

//-------------------------------------------------

//存储每个线程的协程状态
static RoutineEnv* g_arryEnvPerThread[ 204800 ] = {0};
unsigned long long Routine_Id=0;

//-------------RoutineEnv--------------------------

//初始化当前线程的协程环境
//并初始化主协程
void init_curr_thread_env()
{
	pid_t pid = GetPid();	
	g_arryEnvPerThread[ pid ] = new RoutineEnv();
	RoutineEnv *env = g_arryEnvPerThread[ pid ];

	env->CallStackSize_ = 0;
	Routine *self = new Routine( env, NULL, NULL ,NULL );
	self->IsMainRoutine_= 1;

	env->pending_ = NULL;
	env->occupy_ = NULL;


	coctx_init( &self->ctx_ );

	env->CallStack_[ env->CallStackSize_++ ] = self;
    
    //初始化定时器
    env->time_heap_ = new Time_heap(50);
	//stCoEpoll_t *ev = AllocEpoll();
	//SetEpoll( env,ev );
}


//得到当前线程的协程环境,如果没有,则进行初始化
RoutineEnv* get_curr_thread_env()
{
    if(!g_arryEnvPerThread[ GetPid() ])
    {
        init_curr_thread_env(); 
    }
    //std::cout<<"GetPid : "<<GetPid()<<std::endl;
	return g_arryEnvPerThread[ GetPid() ];
}

Routine * get_curr_routine()
{
    RoutineEnv * env = get_curr_thread_env();
    return env->CallStack_[env->CallStackSize_-1];
}




//栈内存
StackMemry::StackMemry(int stack_size)
  :  occupy_co_(NULL),
     stack_size_(stack_size)
{
    stack_buffer_ = (char *) malloc(stack_size_); 
    stack_bp_ = stack_buffer_ + stack_size_;    //栈顶
}




//----------------Routine-----------------
Routine::Routine( RoutineEnv * env, 
                 const RoutineAttr* attr,
                 void* (*pfn)(void*),void *arg )
  : env_(env),
    pfn_(pfn),
    arg_(arg),
    Start_(0),
    End_(0),
    IsMainRoutine_(0),
    Id_(Routine_Id++)
{
    RoutineAttr at ;
    if(attr)
    {
        at = *attr;
    }
    
    //初始化栈大小
	if( at.stack_size_ <= 0 )
	{
		at.stack_size_ = 128 * 1024;
	}
	else if( at.stack_size_ > 1024 * 1024 * 8 )
	{
		at.stack_size_ = 1024 * 1024 * 8;
	}

	if( at.stack_size_ & 0xFFF ) 
	{
		at.stack_size_ &= ~0xFFF;
		at.stack_size_ += 0x1000;
	}
	
    //分配栈内存
    StackMemry* stack_memry = new StackMemry(at.stack_size_);

	stack_memry_ = stack_memry;

    ctx_.ss_sp = stack_memry_->stack_buffer_;
	ctx_.ss_size = at.stack_size_;

	save_size_ = 0;
	save_buffer_ = NULL;

    //----debug-----
    std::cout<<"成功创建了一个协程!"<<std::endl;
}



//执行本协程
void Routine::resume()
{

    RoutineEnv *env = env_;
	Routine *Current_Routine = env->CallStack_[ env->CallStackSize_ - 1 ]; //取当前协程块指针 
    //std::cout<<env->CallStackSize_<<std::endl;
	//协程首次执行,创建其上下文
    if( !Start_ )
	{
		coctx_make( &ctx_,(coctx_pfn_t)RoutineFunc,this,0 );
	    Start_ = 1;
	}
    
	env->CallStack_[ env->CallStackSize_++ ] = this;//将新协程块指针压入pCallStack栈中
	co_swap( Current_Routine, this );     
}

//退出本协程
void Routine::yield()
{
    //确保不重复退出协程
    assert(End_ == 0 && Start_ == 1);
    yield_env(env_);
}


Routine::~Routine()
{

    //std::cout<<" Free Routine " << std::endl;
    free(stack_memry_->stack_buffer_);
    free(stack_memry_);
    Routine_Id--;
}

//退出当前线程中的协程
void yield_current_thread()
{
    yield_env(get_curr_thread_env() );
}

void yield_env(RoutineEnv* env_)
{
	Routine *Last_routine    = env_->CallStack_[ env_->CallStackSize_ - 2 ]; //获取上一个协程块
    Routine *Current_Routine = env_->CallStack_[ env_->CallStackSize_ - 1 ]; //获取当前协程块

	env_->CallStackSize_--;
	co_swap( Current_Routine, Last_routine);
}


//-------------------------------------------





//协程中要执行的函数
//该函数会调用回调函数
//并退出当前协程
static int RoutineFunc( Routine* routine,void * )
{
    //执行协程函数
	if( routine->pfn_ )
	{
		routine->pfn_( routine->arg_ );
	}

	routine->End_ = 1;
	
    RoutineEnv *env = routine->env_;
    //切回上一个栈中的上一个协程
    //while(1)防止被函数执行完后多次调用resume
    while(1) yield_env( env );
	return 0;
}



//交换执行两个协程
void co_swap(Routine* Current_Routine, Routine* Pending_Routine)
{
    RoutineEnv* env = get_curr_thread_env();
    
    //得到当前栈指针
	//get curr stack sp
	char c;
	Current_Routine->stack_sp_= &c;

	env->pending_ = NULL;
	env->occupy_ = NULL;

	//swap context
    coctx_swap(&(Current_Routine->ctx_),&(Pending_Routine->ctx_) );
    
    //防止栈内存被修改
    //重新得到栈内存
	//stack buffer may be overwrite, so get again;
	RoutineEnv* curr_env = get_curr_thread_env();
	Routine* update_occupy_co =  curr_env->occupy_;
    Routine* update_pending_co = curr_env->pending_;
	if (update_occupy_co && update_pending_co && update_occupy_co != update_pending_co)
	{
		//resume stack buffer
		if (update_pending_co->save_buffer_ && update_pending_co->save_size_ > 0)
		{
			memcpy(update_pending_co->stack_sp_, update_pending_co->save_buffer_,\
            update_pending_co->save_size_);
		}
	}
}




//------------协程调度-----------------


//参数1 : 事件定时器
//参数2 : 在EventLoop提供的回调函数
//参数3 : 回调函数的参数
EventLoop::EventLoop( Time_heap* time_heap, EventLoop_Function runInLoopFunction , void *arg )
  : time_heap_(time_heap),
    runInLoopFunction_(runInLoopFunction),
    arg_(arg)
    
{
     
}

EventLoop::~EventLoop()
{

}


//执行EventLoop要执行的函数
void EventLoop::runInLoop()
{
    if(runInLoopFunction_)
    {
        runInLoopFunction_(arg_);
    }
}

void EventLoop::loop()
{
    while(1)
    {
        time_heap_->runOutTimeEvent(); 
        runInLoop();
    }
}





















}

