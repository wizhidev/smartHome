//
// Created by hwp on 2017/11/21.
//

#ifndef SHOME_LIB_SNPKT_H
#define SHOME_LIB_SNPKT_H

#include "../comm/comm.h"
struct _SnPktNode {
  unsigned char gw_id[12];  //设备Id
  int snPkt;
};

typedef struct _SnPktNode SnPktNode;

Ret snPkt_cmp(void *ctx, void *data);

#endif //SHOME_LIB_SNPKT_H
