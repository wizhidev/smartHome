//
// Created by hwp on 2017/6/14.
//

#include <stdlib.h>
#include <string.h>
#include "../pthread/locker.h"
#include "gw_list.h"
#include "gw_node.h"

#define SLIST_TAIL -1

typedef struct _SListNode {
  void *data;
  struct _SListNode *next;
} SListNode;

struct _SList {
  Locker *locker;
  SListNode *first;
  int size;
  DataDestroyFunc data_destroy;
  void *data_destroy_ctx;
} ;

static void slist_lock(SList *thiz){
	if (thiz->locker != NULL){
		locker_lock(thiz->locker);
	}
}

static void slist_unlock(SList *thiz){
	if (thiz->locker != NULL){
		locker_unlock(thiz->locker);
	}
}

static void slist_destroy_locker(SList *thiz){
	if (thiz->locker != NULL){
		locker_unlock(thiz->locker);
		locker_destroy(thiz->locker);
	}
}

static SListNode *slist_create_node(void *data) {
	SListNode *node = (SListNode *) malloc(sizeof(SListNode));

	if (NULL != node) {
		node->data = data;
		node->next = NULL;
	}

	return node;
}

static SListNode *slist_get_prev_node(SList *thiz, size_t index, int fail_return_last) {
	return_val_if_fail(thiz != NULL, NULL);

	SListNode *node = thiz->first;
	index--;
	while (node != NULL && node->next != NULL && index > 0) {
		node = node->next;
		index--;
	}
	if (!fail_return_last) {
		return node = index > 0 ? NULL : node;
	}

	return node;
}

static Ret slist_insert(SList *thiz, size_t index, void *data) {
	SListNode *node = NULL;
	SListNode *cursor = NULL;
	Ret ret = RET_FAIL;

	return_val_if_fail(thiz != NULL, RET_INVALID_PARAMS);
	size_t len = slist_length(thiz);
	slist_lock(thiz);
	do {
		node = slist_create_node(data);
		if (NULL == node) {
			ret = RET_OOM;
			break;
		}

		if (thiz->first == NULL) {
			thiz->first = node;
			ret = RET_OK;
			break;
		}

		cursor = ((index == 0) ? NULL : slist_get_prev_node(thiz, index, 1));
		if (index >= 0 && index < len) {
			//head node
			if (NULL == cursor) {
				cursor = thiz->first;
				thiz->first = node;
				node->next = cursor;
			} else {
				node->next = cursor->next;
				cursor->next = node;
			}
		} else {
			//default insert to tail if not find
			cursor->next = node;
			node->next = NULL;
			ret = RET_OK;
		}
	}while(0);

	slist_unlock(thiz);

	return ret;
}

Ret slist_delete(SList *thiz, size_t index) {
	return_val_if_fail(thiz != NULL, RET_INVALID_PARAMS);

	SListNode *node = NULL;
	SListNode *tmp = NULL;
	Ret ret = RET_OK;
	slist_lock(thiz);

	do {
		if (-1 == index) {
			ret = RET_OK;
		}
		//locate prev node of current node
		node = slist_get_prev_node(thiz, index, 1);
		if (0 == index) {
			node = thiz->first;
			thiz->first = node->next;
			SAFE_FREE(node);
		} else {
			tmp = node->next;
			node->next = node->next->next;
			SAFE_FREE(tmp);
		}
	}while(0);
	slist_unlock(thiz);

	return ret;
}

static Ret slist_get_by_index(SList *thiz, size_t index, void **data) {
	return_val_if_fail(thiz != NULL, 0);

	slist_lock(thiz);

	SListNode *prev_node = NULL;
	if (index == 0) {
		*data = thiz->first->data;
		slist_unlock(thiz);
		return RET_OK;
	}

	prev_node = slist_get_prev_node(thiz, index, 1);
	if (NULL != prev_node) {
		*data = prev_node->next->data;
	}

	slist_unlock(thiz);

	return (prev_node != NULL) ? RET_OK : RET_INVALID_PARAMS;
}

static Ret slist_set_by_index(SList *thiz, size_t index, void *data) {
	return_val_if_fail(thiz != NULL, 0);
	Ret ret = RET_FAIL;

	slist_lock(thiz);

	SListNode *prev_node = NULL;
	if (0 == index) {
		thiz->first->data = data;
		slist_unlock(thiz);
		return ret;
	}

	prev_node = slist_get_prev_node(thiz, index, 1);
	if (prev_node != NULL) {
		prev_node->next->data = data;
	}

	slist_unlock(thiz);

	return ret;
}

size_t slist_find(SList *thiz, DataCompareFunc cmp, void *data) {
	return_val_if_fail(thiz != NULL, 0);
	size_t ret = RET_FAIL;

	slist_lock(thiz);

	size_t index = 0;
	SListNode *node = thiz->first;
	while (node != NULL) {
		if (0 == cmp(node->data, data)) {
			ret = index;
			break;
		}
		node = node->next;
		index++;
	}
	slist_unlock(thiz);

	return ret;
}

size_t slist_length(SList *thiz) {
	return_val_if_fail(thiz != NULL, 0);

	slist_lock(thiz);

	size_t length = 0;
	SListNode *node = thiz->first;

	while (node != NULL) {
		node = node->next;
		length++;
	}

	slist_unlock(thiz);

	return length;
}

Ret slist_get_node(SList *thiz, DataCompareFunc cmp, void *value, void **data){
	size_t index = slist_find(thiz, cmp, value);
	if (index == RET_FAIL){
		return RET_FAIL;
	} else {
		return slist_get_by_index(thiz, index, data);
	}
}

Ret slist_foreach(SList *thiz, DataVisitFunc visit, void *data) {
	return_val_if_fail(thiz != NULL, RET_INVALID_PARAMS);

	SListNode *node = thiz->first;
	while (node != NULL) {
		visit(data, node->data);
		node = node->next;
	}

	return RET_OK;
}

Ret slist_append(SList *thiz, DataCompareFunc cmp, void *data) {
	size_t index = slist_find(thiz, cmp, data);
	if (index == RET_FAIL)
		return slist_insert(thiz, SLIST_TAIL, data);
	else
		return slist_set_by_index(thiz, index, data);
}

SList *slist_create(DataDestroyFunc data_destroy, void *data_destroy_ctx, Locker *locker) {
	SList *thiz = NULL;
	thiz = (SList *) malloc(sizeof(SList));

	if (NULL != thiz) {
		thiz->first = NULL;
		thiz->locker = locker;
		thiz->size = 0;
		thiz->data_destroy = data_destroy;
		thiz->data_destroy_ctx = data_destroy_ctx;
	}

	return thiz;
}