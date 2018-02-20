/*************************************************************************
	> File Name: Conditional_variable.h
	> Author: 
	> Mail: 
	> Created Time: 2018年02月20日 星期二 16时19分35秒
 ************************************************************************/

#ifndef _CONDITIONAL_VARIABLE_H
#define _CONDITIONAL_VARIABLE_H


#include"Time_heap.h"
#include"routine.h"
#include<queue>
#include"Callback.h"

namespace libfly
{



class Conditional_variable
{

public:
    Conditional_variable();
    ~Conditional_variable();
    void wait();
    void notify_one();
    void notify_all();

public:
    std::queue<heap_timer*> routines_;
    int size_;     

};



















}








#endif
