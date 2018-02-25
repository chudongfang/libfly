


#include"routine.h"


namespace libfly
{

class Routine;

//协程函数
typedef void *(*Routine_Function)( void * );


//定时器函数
typedef void *(*TimerCallback)( Routine * );

//EventLoop中执行的函数
typedef int (*EventLoop_Function)(void *);

}

