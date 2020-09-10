//
// Created by hwp on 2017/11/21.
//

#ifndef SHOME_LIB_GW_NODE_H
#define SHOME_LIB_GW_NODE_H

#include <time.h>
#include <netinet/in.h>

struct _GwNode {
  struct sockaddr_in sender;
  unsigned char gw_id[12];  //设备Id
  unsigned char rcu_ip[4];  //内网地址
  time_t seconds;
};
typedef struct _GwNode GwNode;

Ret gw_cmp(void *ctx, void *data);

#endif //SHOME_LIB_GW_NODE_H
