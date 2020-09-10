//
// Created by hwp on 2017/6/21.
//

#ifndef SHOME_LIB_TIME_H
#define SHOME_LIB_TIME_H

#include <stdio.h>

#define N      10
#define TIMER_SEARCH_DEV_SPEC  1
#define TIMER_SEARCH_DEL_GW   2
#define TIMER_SEARCH_DEL_SN   3

extern void *singal_msg(void *arg);
extern void setTimer(int time, int fun);

#endif //SHOME_LIB_TIME_H
