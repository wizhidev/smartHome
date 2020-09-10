//
// Created by hwp on 2017/11/21.
//

#ifndef SHOME_LIB_COMM_H
#define SHOME_LIB_COMM_H
#include <stdio.h>

typedef enum _Ret {
  RET_FAIL = -1,
  RET_OK,
  RET_OOM,
  RET_STOP,
  RET_INVALID_PARAMS,
} Ret;

typedef void     (*DataDestroyFunc)(void *ctx, void *data);

typedef int      (*DataCompareFunc)(void *ctx, void *data);

typedef Ret      (*DataVisitFunc)(void *ctx, void *data);

typedef Ret (*DataSortFunc)(void **array, size_t nr, DataCompareFunc cmp);

typedef void     (*DataExtract)(void *data);

#define return_if_fail(p) if(!(p)) \
    {printf("%s:%d Warning: "#p" failed.\n", \
        __func__, __LINE__); return;}
#define return_val_if_fail(p, ret) if(!(p)) \
    {printf("%s:%d Warning: "#p" failed.\n",\
    __func__, __LINE__); return (ret);}

#define SAFE_FREE(p) if(p != NULL) {free(p); p = NULL;}
#endif //SHOME_LIB_COMM_H
