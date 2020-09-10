//
// Created by hwp on 2017/11/21.
//
#include <netinet/in.h>
#include <memory.h>
#include "../comm/comm.h"
#include "gw_node.h"

Ret gw_cmp(void *ctx, void *data) {
	GwNode *current = (GwNode * )ctx;
	GwNode *node = (GwNode * )data;

	if (current->sender.sin_addr.s_addr == node->sender.sin_addr.s_addr
		&& memcmp(current->rcu_ip, node->rcu_ip, 4) == 0
		&& memcmp(current->gw_id, node->gw_id, 12) == 0) {
		return RET_OK;
	}

	return RET_FAIL;
}