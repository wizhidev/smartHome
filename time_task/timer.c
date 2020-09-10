//
//  timer.c
//  smart_house_server
//
//  Created by hwp on 2017/2/24.
//  Copyright © 2017年 hwp. All rights reserved.
//

#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <memory.h>

#include "timer.h"
#include "../global.h"
#include "../data_list/gw_node.h"
#include "../incharge/format_json_to_bin.h"

int timer_num; //i代表定时器的个数；t表示时间，逐秒递增
struct Timer { //Timer结构体，用来保存一个定时器的信息
  int total_time;  //每隔total_time秒
  int left_time;   //还剩left_time秒
  int func;        //该定时器超时，要执行的代码的标志
} myTimer[N];   //定义Timer类型的数组，用来保存所有的定时器

extern pthread_mutex_t mutex;

Ret no_id_cmp(void *ctx, void *data) {
	GwNode *node = (GwNode * )data;

	get_devs_pkt(node->gw_id);
	sleep(5);
	get_scenes_pkt(node->gw_id);
	sleep(5);
	get_security_pkt(node->gw_id, 3, 255);
	return RET_OK;
}

static Ret get_dev_infos(SList *thiz) {
	slist_foreach(thiz, no_id_cmp, NULL);
}

Ret time_cmp(void *ctx, void *data) {
	GwNode *current = (GwNode * )ctx;
	time_t seconds = (int) time((time_t *) NULL);

	if (seconds - current->seconds > 30*60) {
		return RET_OK;
	}
	return RET_FAIL;
}

static void clear_up_gwlist(SList *thiz) {
	size_t index = slist_find(thiz, time_cmp, NULL);
	if( index != RET_FAIL)
		slist_delete(thiz, index);
}

static void clear_up_snPktlist(SList *thiz) {
	for (int i = 0; i < slist_length(thiz); ++i) {
		slist_delete(thiz, i);
	}
}

static void timeout_fun()  //判断定时器是否超时，以及超时时所要执行的动作
{
	int j;
	for (j = 0; j < timer_num; j++) {
		if (myTimer[j].left_time != 0)
			myTimer[j].left_time--;
		else {
			switch (myTimer[j].func) { //通过匹配myTimer[j].func，判断下一步选择哪种操作
				case TIMER_SEARCH_DEV_SPEC:
					get_dev_infos(gw_list);
					break;
				case TIMER_SEARCH_DEL_GW:
					clear_up_gwlist(gw_list);
					break;
				case TIMER_SEARCH_DEL_SN:
					clear_up_snPktlist(sn_list);
					break;
			}
			myTimer[j].left_time = myTimer[j].total_time;     //循环计时
		}
	}
}

void setTimer(int time, int fun) //新建一个计时器
{
	struct Timer a;
	a.total_time = time;
	a.left_time = time;
	a.func = fun;
	myTimer[timer_num++] = a;
}

void *singal_msg(void *arg) {
	while (1) {
		sleep(1); //每隔一秒发送一个SIGALRM
		timeout_fun();
	}
}
