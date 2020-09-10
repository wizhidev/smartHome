//
// Created by hwp on 2017/11/21.
//


#include <memory.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "../comm/comm.h"
#include "../udp/network.h"
#include "../comm/udppropkt.h"
#include "../comm/log.h"
#include "../data_list/gw_node.h"
#include "../data_list/gw_list.h"
#include "../global.h"
#include "../comm/str_comm.h"
#include "../udp/udp.h"
static int PKT_SN = 0;
static int HANDSHAKE_SN_NUM = 0;
char LOCAL_IP[16] = {0};

Ret get_local_ip() {
	get_ip("v4", LOCAL_IP);
	log_info("本机IP：%s\n", LOCAL_IP);
	if (is_valid_ip(LOCAL_IP)) {
		return RET_OK;
	} else {
		return RET_FAIL;
	}
}

static Ret check_devUnitid(void *ctx, void *data) {
	GwNode *current = (GwNode *) ctx;
	GwNode *check_value = (GwNode *) data;

	if (memcmp(current->gw_id, check_value->gw_id, 12) == 0) {
		return RET_OK;
	}

	return RET_FAIL;
}

static Ret set_dst_ip(UDPPROPKT *pkt, void *data) {
	Ret ret = RET_FAIL;
	GwNode *gwNode = (GwNode *) data;
	if (slist_get_node(gw_list, check_devUnitid, gwNode, (void **) &gwNode) == RET_OK) {
		memcpy(pkt->dstIp, gwNode->rcu_ip, 4);
		ret = RET_OK;
	} else {
		memset(pkt->dstIp, 0x00, 4);
	}

	return ret;
}

static Ret set_src_ip(UDPPROPKT *pkt) {
	unsigned long temp = inet_addr(LOCAL_IP);
	uint8_t *src_ip = (uint8_t *) &temp;

	for (int i = 0; i < 4; i++) {
		pkt->srcIp[i] = src_ip[i];
	}

	return RET_OK;
}

static Ret set_id_pwd(UDPPROPKT *pkt, uint8_t *id) {
	if (id != NULL) {
		memcpy(pkt->uidDst, id, ID_LEN_BYTE);
		uint8_t uid[25] = {0};
		bytes_to_string(id, uid, ID_LEN_BYTE);
		mm_string_cut_by_where_start_and_end(uid, 16, 24, pkt->pwdDst, 0);
	} else {
		memset(pkt->uidDst, 0xff, ID_LEN_BYTE);
	}

	memset(pkt->uidSrc, 0xff, ID_LEN_BYTE);

	return RET_OK;
}

Ret set_pkt_sn(UDPPROPKT *pkt) {
	if (pkt->bAck == NOT_ACK) {
		pkt->snPkt = PKT_SN;
		PKT_SN = pkt->snPkt;
		HANDSHAKE_SN_NUM++;
		if (HANDSHAKE_SN_NUM == 3) {
			PKT_SN = PKT_SN + 1;
		}
	} else {
		pkt->snPkt = PKT_SN + 1;
		PKT_SN = pkt->snPkt + 1;
	}

	return RET_OK;
}

UDPPROPKT *pre_send_udp_pkt(uint8_t *dat, int dat_len, uint8_t cmd, uint8_t *rcu_id,
							int ack, int sub_type1, int sub_type2) {

	UDPPROPKT *pkt = (UDPPROPKT *) malloc(sizeof(UDPPROPKT) + dat_len);

	memcpy(pkt->head, HEAD_STRING, 4);

	GwNode *gwNode = malloc(sizeof(GwNode));
	memcpy(gwNode->gw_id, rcu_id, 12);

	set_dst_ip(pkt, gwNode);
	printf("发送指令:ip:%d,%d.%d.%d.%d\n", cmd, pkt->dstIp[0],pkt->dstIp[1],pkt->dstIp[2],pkt->dstIp[3]);
	set_src_ip(pkt);
	set_id_pwd(pkt, rcu_id);

	pkt->bAck = ack;
	pkt->datType = cmd;
	pkt->subType1 = sub_type1;
	pkt->subType2 = sub_type2;
	pkt->currPkt = 0;
	pkt->sumPkt = 1;
	pkt->datLen = dat_len;

	set_pkt_sn(pkt);

	memcpy(pkt->dat, dat, dat_len);

	free(gwNode);
	return pkt;
}

void send_data_to_gw(uint8_t *devUnitID, void *str, int datlen) {
	GwNode *gwNode = (GwNode *) malloc(sizeof(GwNode));
	memcpy(gwNode->gw_id, devUnitID, 12);
	if (slist_get_node(gw_list, check_devUnitid, gwNode, (void **) &gwNode) == RET_OK) {
		unsigned char idstr[25] = {0};
		bytes_to_string(devUnitID, idstr, 12);
		printf("查询 id 号:%s  ip: %s 端口:%d\n", idstr, inet_ntoa(gwNode->sender.sin_addr), gwNode->sender.sin_port);
		send_inet_udp_server(gwNode->sender, str, datlen);
	}
	//free(gwNode);
	return;
}

void create_udp_broadcast(uint8_t *devUnitID) {
	UDPPROPKT *pkt = pre_send_udp_pkt(NULL, 0, e_udpPro_getRcuInfoNoPwd, devUnitID, IS_ACK, 0, 0);
	udp_broadcast((void *) pkt, UDP_PKT_SIZE);
}

UDPPROPKT *create_handshake_pkt(UDPPROPKT *pkt) {
	return pre_send_udp_pkt(0, 0, e_udpPro_handShake, pkt->uidSrc, NOT_ACK, 0, 0);
}

UDPPROPKT *create_ask_pkt(UDPPROPKT *pkt) {
	pkt->bAck = NOW_ACK;
	uint8_t temp[4];

	memcpy(temp, pkt->dstIp, 4);
	memcpy(pkt->dstIp, pkt->srcIp, 4);
	memcpy(pkt->srcIp, temp, 4);

	return pkt;
}

