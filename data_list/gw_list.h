//
// Created by hwp on 2017/6/14.
//

#ifndef SMHOMEANDROIDLIB_GW_LIST_H
#define SMHOMEANDROIDLIB_GW_LIST_H
#include "../comm/comm.h"
#include "../pthread/locker.h"

struct _SList;
typedef struct _SList SList;

size_t slist_length(SList *thiz);
Ret slist_append(SList *thiz, DataCompareFunc cmp, void *data);
Ret slist_get_node(SList *thiz, DataCompareFunc cmp, void *value, void **data);
size_t slist_find(SList *thiz, DataCompareFunc cmp, void *data);
Ret slist_delete(SList *thiz, size_t index);
Ret slist_foreach(SList *thiz, DataVisitFunc visit, void *data);
SList *slist_create(DataDestroyFunc data_destroy, void *data_destroy_ctx, Locker *locker);

#endif //SMHOMEANDROIDLIB_GW_LIST_H
