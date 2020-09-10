//
// Created by hwp on 2017/11/21.
//

#include <pthread.h>
#include <stdlib.h>
#include "locker.h"

static Ret locker_pthread_lock(Locker *thiz){
	int ret = pthread_mutex_lock(&thiz->mutex);
	return ret == 0 ? RET_OK : RET_FAIL;
}

static Ret locker_pthread_unlock(Locker *thiz){
	int ret = pthread_mutex_unlock(&thiz->mutex);
	return ret == 0 ? RET_OK : RET_FAIL;
}

static void locker_pthread_destroy(Locker *thiz){
	pthread_mutex_destroy(&thiz->mutex);
	SAFE_FREE(thiz);
}

Locker *locker_pthread_create(void){
	Locker *thiz = (Locker *)malloc(sizeof(Locker));

	if (thiz != NULL){

		thiz->lock = locker_pthread_lock;
		thiz->unlock = locker_pthread_unlock;
		thiz->destroy = locker_pthread_destroy;

		pthread_mutex_init(&(thiz->mutex), NULL);
	}

	return thiz;
}