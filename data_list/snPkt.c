//
// Created by hwp on 2017/11/21.
//

#include <string.h>
#include "snPkt.h"

Ret snPkt_cmp(void *ctx, void *data) {
	SnPktNode *current = (SnPktNode * )ctx;
	SnPktNode *node = (SnPktNode * )data;

	if (current->snPkt == node->snPkt
		&& memcmp(current->gw_id, node->gw_id, 12) == 0){
		return RET_OK;
	}

	return RET_FAIL;
}