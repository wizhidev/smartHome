//
// Created by hwp on 2017/11/22.
//

#include <memory.h>
#include <sys/socket.h>
#include <assert.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "../udp/udp.h"
#include "../data_list/snPkt.h"
#include "../data_list/gw_list.h"
#include "../data_list/gw_node.h"
#include "../global.h"
#include "../comm/log.h"
#include "to_gwclient.h"
#include "to_uiclient.h"
#include "../json/cJSON.h"
#include "format_json_to_bin.h"

static void add_gw_tolist(SRequest *request, UDPPROPKT *pkt);
static Ret add_snPkt_tolist(UDPPROPKT *pkt);
static Ret check_invalid_data(UDPPROPKT *pkt);
static void show_debug_info(UDPPROPKT *pkt, SRequest *request);
static void parser_data_cmd(SRequest *request, UDPPROPKT *pkt);

void extract_json(void *data) {

	uint8_t *buffer = (uint8_t *) data;
	uint8_t devUnitID[ID_LEN_BYTE];
	uint8_t devUnitPass[PASS_LEN_STR];
	int devType;
	int devID;

	cJSON *root_json = cJSON_Parse((char *) buffer);
	if (NULL == root_json) {
		cJSON_Delete(root_json);
		return;
	}

	//devUnitID
	uint8_t *devUnitID_str;
	cJSON *devUnitID_json = cJSON_GetObjectItem(root_json, "devUnitID");
	if (devUnitID_json != NULL) {
		devUnitID_str = (uint8_t *) devUnitID_json->valuestring;
		string_to_bytes(devUnitID_str, devUnitID, ID_LEN_BYTE * 2);
	} else {
		return;
	}

	cJSON *devPass_json = cJSON_GetObjectItem(root_json, "devPass");
	if (devPass_json != NULL) {
		char *pass = devPass_json->valuestring;
		memcpy(devUnitPass, pass, PASS_LEN_STR);
	}

	//datType
	int datType = cJSON_GetObjectItem(root_json, "datType")->valueint;

	switch (datType) {
		case e_udpPro_ctrlDev: {
			devType = cJSON_GetObjectItem(root_json, "devType")->valueint;
			devID = cJSON_GetObjectItem(root_json, "devID")->valueint;
			int cmd = cJSON_GetObjectItem(root_json, "cmd")->valueint;

			char *canCpuID_str = cJSON_GetObjectItem(root_json, "canCpuID")->valuestring;
			ctrl_devs_pkt(devUnitID, canCpuID_str, e_udpPro_ctrlDev, devType, devID, 0, 0, 0, cmd, buffer);
		}
			break;
		case e_udpPro_exeSceneEvents: {
			int eventId = cJSON_GetObjectItem(root_json, "eventId")->valueint;
			ctrl_scene_pkt(devUnitID, eventId, datType);
		}
			break;
		case e_udpPro_security_info: {
			int subType1 = cJSON_GetObjectItem(root_json, "subType1")->valueint;
			int subType2 = cJSON_GetObjectItem(root_json, "subType2")->valueint;
			set_security_pkt(devUnitID, subType1, subType2, buffer);
		}
			break;
		default:break;
	}

	cJSON_Delete(root_json);
}

void parser_data_fromui(void *req) {
	extract_json(req);
}

void parser_data_fromgw(void *req) {
	SRequest *request = (SRequest *) req;
	UDPPROPKT *pkt = (UDPPROPKT *) request->buf;
//	log_info("收到的字符串:%s\n", request->buf);
//	send_inet_udp_server(request->from, (uint8_t *)request->buf, strlen(request->buf));
//	return;

	if (RET_FAIL == check_invalid_data(pkt)) {
		//log_debug("invalid data received.");
		return;
	}

//	if (RET_OK == add_snPkt_tolist(pkt)) {
//		//log_info("snPkt list = %d\n", (int) slist_length(sn_list));
//		return;
//	}

	if (pkt->bAck == IS_ACK) { //应答包
		UDPPROPKT *ack_pkt = create_ask_pkt(pkt);
		send_inet_udp_server(request->from, (uint8_t *) ack_pkt, UDP_PKT_SIZE);
	}
	show_debug_info(pkt, request);

	parser_data_cmd(request, pkt);
}

static void add_gw_tolist(SRequest *request, UDPPROPKT *pkt) {
	RCU_INFO *rcu = (RCU_INFO *) pkt->dat;
	GwNode *gwNode = (GwNode *) malloc(sizeof(GwNode));

	gwNode->sender = request->from;
	memcpy(gwNode->gw_id, pkt->uidSrc, 12);
	memcpy(gwNode->rcu_ip, rcu->IpAddr, 4);
	gwNode->seconds = (int) time((time_t *) NULL);

	slist_append(gw_list, gw_cmp, (void *) gwNode);
}

static Ret add_snPkt_tolist(UDPPROPKT *pkt) {

	SnPktNode *snNode = (SnPktNode *) malloc(sizeof(SnPktNode));
	assert(memcpy(snNode->gw_id, pkt->uidSrc, 12));
	snNode->snPkt = pkt->snPkt;

	return slist_append(sn_list, snPkt_cmp, (void *) snNode);
}

static Ret check_invalid_data(UDPPROPKT *pkt) {
	uint8_t data[12] = {0};
	if (memcmp(pkt->uidSrc, data, 12) == 0) {
		return RET_FAIL;
	}
	memset(data, 0xff, 12);
	if (memcmp(pkt->uidSrc, data, 12) == 0) {
		return RET_FAIL;
	}

	return RET_OK;
}

static void show_debug_info(UDPPROPKT *pkt, SRequest *request) {
	uint8_t uidSrc[25] = {0};
	bytes_to_string(pkt->uidSrc, uidSrc, ID_LEN_BYTE);
	if (pkt->datType != 35 && pkt->datType != 2 && pkt->datType != 68)
		log_info("收到的数据包:%d %d %d ack = %d, snPkt = %d bAck = %d uidSrc = %s ip = %s port = %d\n",
				 pkt->datType,
				 pkt->subType1,
				 pkt->subType2,
				 pkt->bAck,
				 pkt->snPkt,
				 pkt->bAck,
				 uidSrc,
				 inet_ntoa(request->from.sin_addr),
				 request->from.sin_port);
}

static void parser_data_cmd(SRequest *request, UDPPROPKT *pkt) {
	switch (pkt->datType) {
		case e_udpPro_getRcuInfo:          // e_udpPro_getRcuinfo
			if (pkt->subType2 == TYPE_STATE_1) {
				add_gw_tolist(request, pkt);
				send_json_info_toui(pkt);
				//log_info("slist size = %d\n", (int) slist_length(gw_list));
			}
			break;
		case e_udpPro_handShake:
			for (int j = 0; j < 3; j++) {//发三遍
				UDPPROPKT *handshake_pkt = create_handshake_pkt(pkt);
				send_inet_udp_server(request->from, (uint8_t *) handshake_pkt, UDP_PKT_SIZE);
			}
			break;
		case e_udpPro_getDevsInfo:
			if (pkt->subType1 == TYPE_STATE_1) {
				send_json_info_toui(pkt); //测试正常
			}
			break;
		case e_udpPro_addDev://测试正常
		case e_udpPro_ctrlDev://测试正常
		case e_udpPro_editDev://测试正常
			if (pkt->subType1 == TYPE_STATE_1) {
				send_json_info_toui(pkt);
			}
			break;
		case e_udpPro_delDev:
			if (pkt->subType1 == TYPE_STATE_1) {
				send_json_info_toui(pkt);
			}
			break;

		case e_udpPro_getSceneEvents:
			if (pkt->subType2 == TYPE_STATE_1) {//测试正常
				send_json_info_toui(pkt);
			}
			break;

		case e_udpPro_addSceneEvents://测试正常
		case e_udpPro_editSceneEvents://测试正常
		case e_udpPro_exeSceneEvents://测试正常
		case e_udpPro_delSceneEvents://测试正常
			if (pkt->subType1 == TYPE_STATE_0 && pkt->subType2 == TYPE_STATE_1) {
				send_json_info_toui(pkt);
			}
			break;
		case e_udpPro_getBoards:
			if (pkt->subType1 == TYPE_STATE_1) {//测试正常
				BOARD_CHNOUT *board = (BOARD_CHNOUT *) pkt->dat; //四种类型结构一样
				switch (board->boardType) {
					case e_board_chnOut: {
						send_json_info_toui(pkt);
					}
						break;
					case e_board_keyInput: {
						send_json_info_toui(pkt);
					}
					default: break;
				}
			}
			break;

		case e_udpPro_getKeyOpItems://测试正常
			if (pkt->subType1 == TYPE_STATE_1) {
				send_json_info_toui(pkt);
			}
			break;
		case e_udpPro_setKeyOpItems://测试正常
			if (pkt->subType1 == TYPE_STATE_1) {
				int result = pkt->dat[12];
				if (result == TYPE_STATE_1) {
					send_json_info_toui(pkt);
				}
			}
			break;
		case e_udpPro_getChnOpItems:
			if (pkt->subType1 == TYPE_STATE_1) {//测试正常
				send_json_info_toui(pkt);
			}
			break;
		case e_udpPro_setChnOpItems:
			if (pkt->subType1 == TYPE_STATE_1) {//测试正常
				send_json_info_toui(pkt);
			}
			break;
		case e_udpPro_security_info: {//测试正常
			if (pkt->bAck != TYPE_STATE_2)
				send_json_info_toui(pkt);
		}
			break;
		case e_udpPro_get_key2scene: {
			if (pkt->subType1 == TYPE_STATE_1 && pkt->subType2 == TYPE_STATE_0)
				send_json_info_toui(pkt);
		}
			break;
		case e_udpPro_set_key2scene: {
			if (pkt->subType1 == TYPE_STATE_1 && pkt->subType2 == TYPE_STATE_0)
				send_json_info_toui(pkt);
		}
			break;
		case e_udpPro_chns_status: {
			send_json_info_toui(pkt);
		}
			break;
		case e_udpPro_getTimerEvents://测试正常
		case e_udpPro_addTimerEvents: //没有该功能
		case e_udpPro_editTimerEvents://测试正常
			if (pkt->subType1 == TYPE_STATE_0 && pkt->subType2 == TYPE_STATE_1) {
				send_json_info_toui(pkt);
			}
			break;
		case e_udpPro_getEnvEvents://测试正常
		case e_udpPro_editEnvEvents://测试正常
			if (pkt->subType1 == TYPE_STATE_0 && pkt->subType2 == TYPE_STATE_1) {
				send_json_info_toui(pkt);
			}
			break;
		case e_udpPro_secs_trigger:
			if (pkt->bAck != TYPE_STATE_2)
				send_json_info_toui(pkt);
			break;
		default:break;
	}
}
