/*************************************************************************
	> File Name: Epoller.h
	> Author: chudongfang
	> Git: https://github.com/chudongfang
	> Created Time: 2018年02月21日 星期三 17时43分20秒
 ************************************************************************/

#ifndef _EPOLLER_H
#define _EPOLLER_H
#include"Epoll.h"


class Epoller
{
    Epoller();



public:
    struct epoll_event events__; 
    Epoll * epoll_;
    int timeout_;
}







#endif
