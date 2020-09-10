//
// Created by hwp on 2017/11/23.
//

#include <memory.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "format_json_to_bin.h"
#include "../comm/udppropkt.h"
#include "../comm/str_comm.h"
#include "../comm/comm.h"
#include "../udp/network.h"
#include "../comm/log.h"
#include "../data_list/gw_node.h"
#include "../data_list/gw_list.h"
#include "../global.h"
#include "../json/cJSON.h"
#include "to_gwclient.h"

void get_rcu_pkt(uint8_t *devUnitID) {
	int len = UDP_PKT_SIZE;
	UDPPROPKT *send_pkt = pre_send_udp_pkt(0, 0, e_udpPro_getRcuInfo, devUnitID, IS_ACK, 0, 0);

	send_data_to_gw(devUnitID, send_pkt, len);
}

void set_rcuInfo_pkt(uint8_t *devUnitID,
					 uint8_t *devUnitPass,
					 uint8_t *name,
					 uint8_t *IpAddr,
					 uint8_t *SubMask,
					 uint8_t *Gateway,
					 uint8_t *centerServ,
					 uint8_t *roomNum,
					 uint8_t *macAddr,
					 int bDhcp) {
	uint8_t data_buf[RCU_INFO_SIZE];
	memset(data_buf, 0, RCU_INFO_SIZE);
	RCU_INFO rcu;
	memcpy(rcu.devUnitID, devUnitID, ID_LEN_BYTE);
	memcpy(rcu.devUnitPass, devUnitPass, PASS_LEN_STR);
	memcpy(rcu.name, name, NAME_LEN_BYTE);
	memcpy(rcu.IpAddr, IpAddr, IP_LEN_BYTE);
	memcpy(rcu.SubMask, SubMask, IP_LEN_BYTE);
	memcpy(rcu.Gateway, Gateway, IP_LEN_BYTE);
	memcpy(rcu.centerServ, centerServ, IP_LEN_BYTE);
	memcpy(rcu.roomNum, roomNum, IP_LEN_BYTE);
	memcpy(rcu.macAddr, macAddr, MAC_LEN_BYTE);
	rcu.bDhcp = bDhcp;

	memcpy(data_buf, &rcu, RCU_INFO_SIZE);

	UDPPROPKT *send_pkt = pre_send_udp_pkt(data_buf,
										   RCU_INFO_SIZE, e_udpPro_setRcuInfo, devUnitID,
										   IS_ACK, 0, 0);
	int len = UDP_PKT_SIZE + RCU_INFO_SIZE;

	send_data_to_gw(devUnitID, send_pkt, len);
}

void get_devs_pkt(uint8_t *devUnitID) {
	int len = UDP_PKT_SIZE;
	UDPPROPKT *send_pkt = pre_send_udp_pkt(0, 0, e_udpPro_getDevsInfo, devUnitID, IS_ACK, 0, 0);

	unsigned char str[150] = {0};
	bytes_to_string((uint8_t *)send_pkt, str, len);
	printf("str = %s\n", str);

	send_data_to_gw(devUnitID, send_pkt, len);
}

void ctrl_devs_pkt(uint8_t *devUnitID,
				   uint8_t *canCpuID,
				   int datType,
				   int devType,
				   int devID,
				   int powChn,
				   uint8_t *devName,
				   uint8_t *roomName,
				   int cmd,
				   char *buffer) {

	int value = -1;
	WARE_DEV ware_dev;
	if (datType == e_udpPro_ctrlDev)
		value = cmd;
	else if (datType == e_udpPro_editDev) {
		value = devType;
	}
	if (datType == e_udpPro_editDev || datType == e_udpPro_addDev) {
		string_to_bytes(devName, ware_dev.devName, strlen(devName));
		string_to_bytes(roomName, ware_dev.roomName, strlen(roomName));
	}

	string_to_bytes(canCpuID, ware_dev.canCpuId, 24);
	ware_dev.devType = devType;
	ware_dev.devId = devID;

	switch (devType) {
		case e_ware_airCond: {
			DEV_PRO_AIRCOND air;
			memset(&air, 0, sizeof(DEV_PRO_AIRCOND));
			air.powChn = powChn;

			uint8_t data_buf[WARE_AIR_SIZE];
			memcpy(data_buf, &ware_dev, WARE_DEV_SIZE);
			memcpy(data_buf + WARE_DEV_SIZE, &air, AIR_SIZE);

			UDPPROPKT *send_pkt = pre_send_udp_pkt(
				data_buf, WARE_AIR_SIZE, datType, devUnitID, IS_ACK, 0, value);
			int len = UDP_PKT_SIZE + WARE_AIR_SIZE;
			send_data_to_gw(devUnitID, send_pkt, len);
		}
			break;
		case e_ware_light: {
			DEV_PRO_LIGHT light;
			memset(&light, 0, sizeof(DEV_PRO_LIGHT));
			light.powChn = powChn;

			uint8_t data_buf[WARE_LGT_SIZE];
			memcpy(data_buf, &ware_dev, WARE_DEV_SIZE);
			memcpy(data_buf + WARE_DEV_SIZE, &light, LIGHT_SIZE);

			UDPPROPKT *send_pkt = pre_send_udp_pkt(
				data_buf, WARE_LGT_SIZE, datType, devUnitID, IS_ACK, 0, value);
			int len = UDP_PKT_SIZE + WARE_LGT_SIZE;
			send_data_to_gw(devUnitID, send_pkt, len);
		}
			break;
		case e_ware_curtain: {
			DEV_PRO_CURTAIN curtain;
			memset(&curtain, 0, sizeof(DEV_PRO_CURTAIN));
			curtain.powChn = powChn;

			uint8_t data_buf[WARE_CUR_SIZE];
			memcpy(data_buf, &ware_dev, WARE_DEV_SIZE);
			memcpy(data_buf + WARE_DEV_SIZE, &curtain, CURTAIN_SIZE);

			UDPPROPKT *send_pkt = pre_send_udp_pkt(
				data_buf, WARE_CUR_SIZE, datType, devUnitID, IS_ACK, 0, value);
			int len = UDP_PKT_SIZE + WARE_CUR_SIZE;
			send_data_to_gw(devUnitID, send_pkt, len);

		}
			break;
		case
			e_ware_tv: {
			uint8_t data_buf[WARE_TV_SIZE];
			memcpy(data_buf, &ware_dev, WARE_TV_SIZE);

			UDPPROPKT *send_pkt = pre_send_udp_pkt(
				data_buf, WARE_TV_SIZE, datType, devUnitID, IS_ACK, 0, value);
			int len = UDP_PKT_SIZE + WARE_TV_SIZE;
			send_data_to_gw(devUnitID, send_pkt, len);
		}
			break;
		case
			e_ware_tvUP: {
			uint8_t data_buf[WARE_TVUP_SIZE];
			memcpy(data_buf, &ware_dev, WARE_TVUP_SIZE);

			UDPPROPKT *send_pkt = pre_send_udp_pkt(
				data_buf, WARE_TVUP_SIZE, datType, devUnitID, IS_ACK, 0, value);
			int len = UDP_PKT_SIZE + WARE_TVUP_SIZE;
			send_data_to_gw(devUnitID, send_pkt, len);
		}
			break;
		case
			e_ware_lock: {
			DEV_PRO_LOCK lock;
			memset(&lock, 0, sizeof(DEV_PRO_LOCK));
			lock.powChnOpen = powChn;

			uint8_t data_buf[WARE_LOCK_SIZE];
			memcpy(data_buf, &ware_dev, WARE_LOCK_SIZE);
			memcpy(data_buf + WARE_DEV_SIZE, &lock, LOCK_SIZE);

			UDPPROPKT *send_pkt = pre_send_udp_pkt(
				data_buf, WARE_LOCK_SIZE, datType, devUnitID, IS_ACK, 0, value);
			int len = UDP_PKT_SIZE + WARE_LOCK_SIZE;
			send_data_to_gw(devUnitID, send_pkt, len);

		}
			break;
		case
			e_ware_fresh_air: {
			//è§£æžJSONæ•°æ®

			DEV_PRO_FRESHAIR frair;
			frair.onOffChn = powChn;
			if (datType == e_udpPro_editDev || datType == e_udpPro_addDev) {
				cJSON *root_json = cJSON_Parse((char *) buffer);    //å°†å­—ç¬¦ä¸²è§£æžæˆjsonç»“æž„ä½“
				if (NULL == root_json) {
					cJSON_Delete(root_json);
					return;
				}

				frair.spdLowChn = cJSON_GetObjectItem(root_json, "spdLowChn")->valueint;
				frair.spdMidChn = cJSON_GetObjectItem(root_json, "spdMidChn")->valueint;
				frair.spdHighChn = cJSON_GetObjectItem(root_json, "spdHighChn")->valueint;
				frair.autoRun = cJSON_GetObjectItem(root_json, "autoRun")->valueint;
				frair.valPm10 = cJSON_GetObjectItem(root_json, "valPm10")->valueint;
				frair.valPm25 = cJSON_GetObjectItem(root_json, "valPm25")->valueint;
			}
			uint8_t data_buf[WARE_FRAIR_SIZE];
			memcpy(data_buf, &ware_dev, WARE_FRAIR_SIZE);
			memcpy(data_buf + WARE_DEV_SIZE, &frair, FRAIR_SIZE);

			UDPPROPKT *send_pkt = pre_send_udp_pkt(
				data_buf, WARE_FRAIR_SIZE, datType, devUnitID, IS_ACK, 0, value);
			int len = UDP_PKT_SIZE + WARE_FRAIR_SIZE;
			send_data_to_gw(devUnitID, send_pkt, len);

		}
			break;
		case
			e_ware_value: {
			DEV_PRO_VALVE valve;
			valve.powChnOpen = powChn;

			uint8_t data_buf[WARE_VALUE_SIZE];
			memcpy(data_buf, &ware_dev, WARE_VALUE_SIZE);
			memcpy(data_buf + WARE_DEV_SIZE, &valve, VALUE_SIZE);

			UDPPROPKT *send_pkt = pre_send_udp_pkt(
				data_buf, WARE_VALUE_SIZE, datType, devUnitID, IS_ACK, 0, value);
			int len = UDP_PKT_SIZE + WARE_VALUE_SIZE;
			send_data_to_gw(devUnitID, send_pkt, len);

		}
			break;
		case e_ware_floor_heat: {
			DEV_PRO_FLOOR_HEAT floor_heat;
			floor_heat.powChn = powChn;

			if (datType == e_udpPro_editDev || datType == e_udpPro_addDev) {
				cJSON *root_json = cJSON_Parse((char *) buffer);    //å°†å­—ç¬¦ä¸²è§£æžæˆjsonç»“æž„ä½“
				if (NULL == root_json) {
					cJSON_Delete(root_json);
					return;
				}
				floor_heat.tempSet = cJSON_GetObjectItem(root_json, "tempSet")->valueint;
				floor_heat.autoRun = cJSON_GetObjectItem(root_json, "autoRun")->valueint;
			}
			uint8_t data_buf[WARE_FLOOR_HEAT_SIZE];
			memcpy(data_buf, &ware_dev, WARE_FLOOR_HEAT_SIZE);
			memcpy(data_buf + WARE_DEV_SIZE, &floor_heat, FLOOR_HEAT_SIZE);

			UDPPROPKT *send_pkt = pre_send_udp_pkt(
				data_buf, WARE_FLOOR_HEAT_SIZE, datType, devUnitID, IS_ACK, 0, value);
			int len = UDP_PKT_SIZE + WARE_FLOOR_HEAT_SIZE;
			send_data_to_gw(devUnitID, send_pkt, len);
		}
			break;
		default:break;
	}
}

void get_scenes_pkt(uint8_t *devUnitID) {
	int len = UDP_PKT_SIZE;
	UDPPROPKT *send_pkt = pre_send_udp_pkt(0, 0, e_udpPro_getSceneEvents, devUnitID, IS_ACK, 0, 0);

	send_data_to_gw(devUnitID, send_pkt, len);
}

void add_scene_pkt(uint8_t *devUnitID, char *sceneName, int devCnt, int eventId, cJSON *item, int cmd) {
	SCENE_EVENT scene;

	memset(scene.sceneName, 0, NAME_LEN_BYTE);
	string_to_bytes(sceneName, scene.sceneName, strlen(sceneName));
	scene.devCnt = devCnt;
	scene.eventId = eventId;
//	scene.rev2 = 0;
//	scene.rev3 = 0;

	int j = 0;
	if (devCnt > 0) {
		cJSON *tasklist = item->child;//å­å¯¹è±¡
		while (tasklist != NULL) {
			char *uid = cJSON_GetObjectItem(tasklist, "canCpuID")->valuestring;
			string_to_bytes(uid, scene.itemAry[j].uid, 24);
			int devType = cJSON_GetObjectItem(tasklist, "devType")->valueint;
			scene.itemAry[j].devType = devType;
			int devID = cJSON_GetObjectItem(tasklist, "devID")->valueint;
			scene.itemAry[j].devID = devID;
			int lmVal = cJSON_GetObjectItem(tasklist, "lmVal")->valueint;
			scene.itemAry[j].lmVal = lmVal;
			scene.itemAry[j].rev2 = 0;
			scene.itemAry[j].rev3 = 0;
			int bOnOff = cJSON_GetObjectItem(tasklist, "bOnOff")->valueint;
			scene.itemAry[j].bOnoff = bOnOff;
			scene.itemAry[j].param1 = 0;
			scene.itemAry[j].param2 = 0;

			tasklist = tasklist->next;
			j++;
		}
	}
	int data_len = 16 + 20 * scene.devCnt;
	uint8_t data_buf[SCENE_SIZE];
	memset(data_buf, 0, SCENE_SIZE);
	memcpy(data_buf, &scene, data_len);

	UDPPROPKT *send_pkt = pre_send_udp_pkt(data_buf, SCENE_SIZE, cmd, devUnitID, IS_ACK, 0, 0);
	int len = UDP_PKT_SIZE + SCENE_SIZE;
	send_data_to_gw(devUnitID, send_pkt, len);
}

void ctrl_scene_pkt(uint8_t *devUnitID, int eventId, int cmd) {
	//devUnitID, eventId
	SCENE_EVENT scene;
	scene.eventId = eventId;
	uint8_t data_buf[SCENE_SIZE];
	memcpy(data_buf, &scene, SCENE_SIZE);

	UDPPROPKT *send_pkt = pre_send_udp_pkt(data_buf, SCENE_SIZE, cmd, devUnitID, IS_ACK, 0, 0);
	int len = UDP_PKT_SIZE + SCENE_SIZE;
	send_data_to_gw(devUnitID, send_pkt, len);
}

void get_board_pkt(uint8_t *devUnitID) {
	int len = UDP_PKT_SIZE;
	UDPPROPKT *send_pkt = pre_send_udp_pkt(0, 0, e_udpPro_getBoards, devUnitID, IS_ACK, 0, 0);

	send_data_to_gw(devUnitID, send_pkt, len);
}

void edit_broad_json(uint8_t *devUnitID, cJSON *item) {

	BOARD_KEYINPUT input;

	memset(input.boardName, 0, NAME_LEN_BYTE);
	memset(input.roomName, 0, NAME_LEN_BYTE);
	memset(input.devUnitID, 0, ID_LEN_BYTE);
	memset(&input, 0, sizeof(BOARD_KEYINPUT));
	cJSON *root_json = item->child;
	if (root_json != NULL) {
		char *devUnitID_str = cJSON_GetObjectItem(root_json, "canCpuID")->valuestring;
		char *boardName_str = cJSON_GetObjectItem(root_json, "boardName")->valuestring;
		char *roomName_str = cJSON_GetObjectItem(root_json, "roomName")->valuestring;
		string_to_bytes(devUnitID_str, input.devUnitID, 24);
		string_to_bytes(boardName_str, input.boardName, strlen(boardName_str));
		string_to_bytes(roomName_str, input.roomName, strlen(roomName_str));
		input.boardType = 1;
		int bResetKey = cJSON_GetObjectItem(root_json, "bResetKey")->valueint;
		input.bResetKey = bResetKey;
		int keyCnt = cJSON_GetObjectItem(root_json, "keyCnt")->valueint;
		input.keyCnt = keyCnt;
		int ledBkType = cJSON_GetObjectItem(root_json, "ledBkType")->valueint;
		input.ledBkType = ledBkType;
		cJSON *array = cJSON_GetObjectItem(root_json, "keyName_rows");
		int iSize = cJSON_GetArraySize(array);
		for (int iCnt = 0; iCnt < iSize; iCnt++) {
			cJSON *pSub = cJSON_GetArrayItem(array, iCnt);
			if (NULL == pSub) {
				continue;
			}
			char *iValue = pSub->valuestring;
			printf("value[%2d] : [%s]\n", iCnt, iValue);
			uint8_t valueName[NAME_LEN_BYTE] = {0};
			string_to_bytes(iValue, valueName, strlen(iValue));
			for (int i = 0; i < NAME_LEN_BYTE; ++i) {
				input.keyName[iCnt][i] = valueName[i];
			}
		}
		int datlen = sizeof(BOARD_KEYINPUT);
		uint8_t data_buf[datlen];
		memset(data_buf, 0, datlen);
		memcpy(data_buf, &input, datlen);

		UDPPROPKT *send_pkt = pre_send_udp_pkt(data_buf, datlen, e_udpPro_editBoards, devUnitID, IS_ACK, 0, 1);

		int len = UDP_PKT_SIZE + datlen;
		send_data_to_gw(devUnitID, send_pkt, len);
	}
}

void get_keyOpitem_pkt(uint8_t *devUnitID, int key_index, uint8_t *uid) {

	uint8_t data_buf[ID_LEN_BYTE];
	memcpy(data_buf, uid, ID_LEN_BYTE);

	UDPPROPKT
		*send_pkt = pre_send_udp_pkt(data_buf, ID_LEN_BYTE, e_udpPro_getKeyOpItems, devUnitID, IS_ACK, 0, key_index);
	int len = UDP_PKT_SIZE + ID_LEN_BYTE;
	send_data_to_gw(devUnitID, send_pkt, len);
}

void get_chnOpitem_pkt(uint8_t *devUnitID, int devType, int devID, uint8_t *uid) {
	uint8_t data_buf[14];
	memcpy(data_buf, uid, ID_LEN_BYTE);
	data_buf[12] = (uint8_t) devType;
	data_buf[13] = (uint8_t) devID;

	UDPPROPKT *send_pkt = pre_send_udp_pkt(data_buf, 14, e_udpPro_getChnOpItems, devUnitID, IS_ACK, 0, 0);
	int len = UDP_PKT_SIZE + 14;
	send_data_to_gw(devUnitID, send_pkt, len);
}

void set_chn_opitem_pkt(uint8_t *devUnitID, uint8_t *canCpuID, int devType, int devID, int cnt,
						cJSON *chnop_item, int cmd) {

	int data_len = 14 + CHN_OPITEM_SIZE * cnt;
	uint8_t data_buf[data_len];
	memset(data_buf, 0, data_len);

	memcpy(data_buf, canCpuID, ID_LEN_BYTE);
	data_buf[12] = devType;
	data_buf[13] = devID;

	if (cnt > 0) {
		int j = 0;
		cJSON *tasklist = chnop_item->child;//å­å¯¹è±¡
		while (tasklist != NULL) {
			CHNOP_ITEM item;

			char *uid = cJSON_GetObjectItem(tasklist, "key_cpuCanID")->valuestring;
			string_to_bytes(uid, item.devUnitID, 24);

			int keyDownValid = cJSON_GetObjectItem(tasklist, "keyDownValid")->valueint;
			item.keyDownValid = keyDownValid;
			int keyUpValid = cJSON_GetObjectItem(tasklist, "keyUpValid")->valueint;
			item.keyUpValid = keyUpValid;
			item.rev1 = 0;
			item.rev2 = 0;
			item.rev3 = 0;

			cJSON *keyDownCmd = cJSON_GetObjectItem(tasklist, "keyDownCmd");
			cJSON *keyUpCmd = cJSON_GetObjectItem(tasklist, "keyUpCmd");
			for (int k = 0; k < 6; k++) {
				item.keyDownCmd[k] = cJSON_GetArrayItem(keyDownCmd, k)->valueint;
				item.keyUpCmd[k] = cJSON_GetArrayItem(keyUpCmd, k)->valueint;
			}

			memcpy(data_buf + 14 + j * CHN_OPITEM_SIZE, &item, CHN_OPITEM_SIZE);
			j++;
			tasklist = tasklist->next;
		}

		UDPPROPKT *send_pkt = pre_send_udp_pkt(data_buf, data_len, cmd, devUnitID, IS_ACK, 0, cnt);
		int len = UDP_PKT_SIZE + data_len;
		send_data_to_gw(devUnitID, send_pkt, len);
	}
}

void set_key_opitem_pkt(uint8_t *devUnitID, uint8_t *canCpuID, int key_index, int cnt, cJSON *keyop_item,
						int cmd) {

	int data_len = 12 + KEY_OPITEM_SIZE * cnt;
	uint8_t data_buf[data_len];
	memcpy(data_buf, canCpuID, ID_LEN_BYTE);

	cJSON *tasklist = keyop_item->child;//å­å¯¹è±¡
	int j = 0;
	while (tasklist != NULL) {
		KEYOP_ITEM item;
		char *uid = cJSON_GetObjectItem(tasklist, "out_cpuCanID")->valuestring;
		string_to_bytes(uid, item.devUnitID, 24);

		int devType = cJSON_GetObjectItem(tasklist, "devType")->valueint;
		item.devType = devType;

		int devID = cJSON_GetObjectItem(tasklist, "devID")->valueint;
		item.devId = devID;

		int keyOpCmd = cJSON_GetObjectItem(tasklist, "keyOpCmd")->valueint;
		item.keyOpCmd = keyOpCmd;

		int keyOp = cJSON_GetObjectItem(tasklist, "keyOp")->valueint;
		item.keyOp = keyOp;

		memcpy(data_buf + ID_LEN_BYTE + j * KEY_OPITEM_SIZE, &item, KEY_OPITEM_SIZE);
		j++;
		tasklist = tasklist->next;
	}

	UDPPROPKT *send_pkt = pre_send_udp_pkt(data_buf, data_len, cmd, devUnitID, IS_ACK, 0, key_index);
	int len = UDP_PKT_SIZE + data_len;
	send_data_to_gw(devUnitID, send_pkt, len);
}

void get_security_pkt(uint8_t *devUnitID, int subType1, int subType2) {
	int len = UDP_PKT_SIZE;
	UDPPROPKT *send_pkt = pre_send_udp_pkt(0, 0, e_udpPro_security_info, devUnitID, IS_ACK, subType1, subType2);

	send_data_to_gw(devUnitID, send_pkt, len);
}

void set_security_pkt(uint8_t *devUnitID, int subType1, int subType2, char *buffer) {

	int data_len = sizeof(SEC_INFO);
	uint8_t data_buf[data_len];
	memset(data_buf, 0, data_len);

	cJSON *root_json = cJSON_Parse((char *) buffer);    //å°†å­—ç¬¦ä¸²è§£æžæˆjsonç»“æž„ä½“
	if (NULL == root_json) {
		printf("error:%s\n", cJSON_GetErrorPtr());
		cJSON_Delete(root_json);
		return;
	}
	if (subType1 == 5 || subType1 == 6 || subType1 == 7) {
		cJSON *security_rows = cJSON_GetObjectItem(root_json, "sec_info_rows");
		cJSON *security_list = security_rows->child;//å­å¯¹è±¡
		if (security_list != NULL) {

			uint8_t *secName = (uint8_t *) cJSON_GetObjectItem(security_list, "secName")->valuestring;
			uint8_t *secCode = (uint8_t *) cJSON_GetObjectItem(security_list, "secCode")->valuestring;
			int secType = cJSON_GetObjectItem(security_list, "secType")->valueint;
			int secId = cJSON_GetObjectItem(security_list, "secId")->valueint;
			int sceneId = cJSON_GetObjectItem(security_list, "sceneId")->valueint;
			int secDev = cJSON_GetObjectItem(security_list, "secDev")->valueint;
			int itemCnt = cJSON_GetObjectItem(security_list, "devCnt")->valueint;
			int valid = cJSON_GetObjectItem(security_list, "valid")->valueint;

			cJSON *itemAry = cJSON_GetObjectItem(security_list, "run_dev_item");

			SEC_INFO sec_info;
			memset(sec_info.secName, 0, NAME_LEN_BYTE);
			string_to_bytes(secName, sec_info.secName, strlen(secName));
			string_to_bytes(secCode, sec_info.secCode, 24);
			sec_info.secType = secType;
			sec_info.secId = secId;
			sec_info.sceneId = sceneId;
			sec_info.secDev = secDev;
			sec_info.itemCnt = itemCnt;
			sec_info.rev1 = 0;
			sec_info.rev2 = 0;
			sec_info.valid = valid;

			if (itemCnt > 0) {
				cJSON *tasklist = itemAry->child;//å­å¯¹è±¡
				int j = 0;
				while (tasklist != NULL) {
					uint8_t *uid = (uint8_t *) cJSON_GetObjectItem(tasklist, "canCpuID")->valuestring;
					string_to_bytes(uid, sec_info.itemAry[j].uid, 24);
					int devType = cJSON_GetObjectItem(tasklist, "devType")->valueint;
					sec_info.itemAry[j].devType = devType;
					int devID = cJSON_GetObjectItem(tasklist, "devID")->valueint;
					sec_info.itemAry[j].devID = devID;
					int lmVal = cJSON_GetObjectItem(tasklist, "lmVal")->valueint;
					sec_info.itemAry[j].lmVal = lmVal;
					sec_info.itemAry[j].rev2 = 0;
					sec_info.itemAry[j].rev3 = 0;
					int bOnOff = cJSON_GetObjectItem(tasklist, "bOnOff")->valueint;
					sec_info.itemAry[j].bOnoff = bOnOff;
					sec_info.itemAry[j].param1 = 0;
					sec_info.itemAry[j].param2 = 0;

					tasklist = tasklist->next;
					j++;
				}
			}
			memcpy(data_buf, &sec_info, sizeof(SEC_INFO));

			UDPPROPKT *send_pkt = pre_send_udp_pkt(data_buf, data_len, e_udpPro_security_info, devUnitID, IS_ACK,
												   subType1, subType2);
			int len = UDP_PKT_SIZE + data_len;
			send_data_to_gw(devUnitID, send_pkt, len);
		}
	}
}

void get_key2scene_pkt(uint8_t *devUnitID) {
	int len = UDP_PKT_SIZE;
	UDPPROPKT *send_pkt = pre_send_udp_pkt(0, 0, e_udpPro_get_key2scene, devUnitID, IS_ACK, 0, 0);

	send_data_to_gw(devUnitID, send_pkt, len);
}

void set_key2scene_pkt(uint8_t *devUnitID, int cnt, cJSON *key2scene_item) {

	int len = UDP_PKT_SIZE + sizeof(KEY2SCENE);

	int data_len = sizeof(KEY2SCENE);
	uint8_t data_buf[data_len];
	memset(data_buf, 0, data_len);

	KEY2SCENE key2scene;
	memcpy(key2scene.flg, "init", 4);

	if (cnt > 0) {
		cJSON *tasklist = key2scene_item->child;//å­å¯¹è±¡
		int j = 0;
		while (tasklist != NULL) {
			char *uid = cJSON_GetObjectItem(tasklist, "canCpuID")->valuestring;
			string_to_bytes(uid, key2scene.items[j].keyUId, 24);
			int keyIndex = cJSON_GetObjectItem(tasklist, "keyIndex")->valueint;
			key2scene.items[j].keyIndex = keyIndex;
			int eventId = cJSON_GetObjectItem(tasklist, "eventId")->valueint;
			key2scene.items[j].eventId = eventId;
			key2scene.items[j].valid = 1;
			key2scene.items[j].rev3 = 0;

			tasklist = tasklist->next;
			j++;
		}
	}
	memcpy(data_buf, &key2scene, data_len);

	UDPPROPKT *send_pkt = pre_send_udp_pkt(data_buf, data_len, e_udpPro_set_key2scene, devUnitID, IS_ACK, 0, 0);

	send_data_to_gw(devUnitID, send_pkt, len);
}

void get_timerEvent_pkt(uint8_t *devUnitID) {
	int len = UDP_PKT_SIZE;
	UDPPROPKT *send_pkt = pre_send_udp_pkt(0, 0, e_udpPro_getTimerEvents, devUnitID, IS_ACK, 0, 0);

	send_data_to_gw(devUnitID, send_pkt, len);
}

void set_timerEvent_pkt(uint8_t *devUnitID, char *buffer) {

	cJSON *root_json = cJSON_Parse((char *) buffer);    //å°†å­—ç¬¦ä¸²è§£æžæˆjsonç»“æž„ä½“
	if (NULL == root_json) {
		cJSON_Delete(root_json);
		return;
	}

	cJSON *timerEvent_rows = cJSON_GetObjectItem(root_json, "timerEvent_rows");
	cJSON *event_list = timerEvent_rows->child;//å­å¯¹è±¡
	if (event_list != NULL) {

		char *timerName = cJSON_GetObjectItem(event_list, "timerName")->valuestring;
		int devCnt = cJSON_GetObjectItem(event_list, "devCnt")->valueint;
		int eventId = cJSON_GetObjectItem(event_list, "eventId")->valueint;
		int valid = cJSON_GetObjectItem(event_list, "valid")->valueint;
		cJSON *run_dev_item = cJSON_GetObjectItem(event_list, "run_dev_item");
		cJSON *timSta = cJSON_GetObjectItem(event_list, "timSta");
		cJSON *timEnd = cJSON_GetObjectItem(event_list, "timEnd");

		int data_len = sizeof(TIMER_EVENT);
		uint8_t data_buf[data_len];
		memset(data_buf, 0, data_len);

		TIMER_EVENT timer_event;
		memset(timer_event.timerName, 0, NAME_LEN_BYTE);
		string_to_bytes(timerName, timer_event.timerName, strlen(timerName));
		timer_event.devCnt = devCnt;
		timer_event.eventId = eventId;
		timer_event.valid = valid;
		timer_event.rev3 = 0;

		for (int k = 0; k < 4; k++) {
			timer_event.timSta[k] = cJSON_GetArrayItem(timSta, k)->valueint;
			timer_event.timEnd[k] = cJSON_GetArrayItem(timEnd, k)->valueint;
		}

		if (devCnt > 0) {
			cJSON *tasklist = run_dev_item->child;//å­å¯¹è±¡
			int j = 0;
			while (tasklist != NULL) {
				uint8_t *uid = (uint8_t *) cJSON_GetObjectItem(tasklist, "canCpuID")->valuestring;
				string_to_bytes(uid, timer_event.item[j].uid, 24);
				int devType = cJSON_GetObjectItem(tasklist, "devType")->valueint;
				timer_event.item[j].devType = devType;
				int devID = cJSON_GetObjectItem(tasklist, "devID")->valueint;
				timer_event.item[j].devID = devID;
				int lmVal = cJSON_GetObjectItem(tasklist, "lmVal")->valueint;
				timer_event.item[j].lmVal = lmVal;
				timer_event.item[j].rev2 = 0;
				timer_event.item[j].rev3 = 0;
				int bOnOff = cJSON_GetObjectItem(tasklist, "bOnOff")->valueint;
				timer_event.item[j].bOnoff = bOnOff;
				timer_event.item[j].param1 = 0;
				timer_event.item[j].param2 = 0;

				tasklist = tasklist->next;
				j++;
			}
		}
		memcpy(data_buf, &timer_event, sizeof(TIMER_EVENT));
		int len = UDP_PKT_SIZE + data_len;
		UDPPROPKT *send_pkt = pre_send_udp_pkt(data_buf, data_len, e_udpPro_editTimerEvents, devUnitID,
											   IS_ACK, 0, 0);

		send_data_to_gw(devUnitID, send_pkt, len);
	}
}

void get_EnvEvents_pkt(uint8_t *devUnitID) {
	int len = UDP_PKT_SIZE;
	UDPPROPKT *send_pkt = pre_send_udp_pkt(0, 0, e_udpPro_getEnvEvents, devUnitID, IS_ACK, 0, 0);

	send_data_to_gw(devUnitID, send_pkt, len);
}

void set_envEvent_pkt(uint8_t *devUnitID, char *buffer) {

	cJSON *root_json = cJSON_Parse((char *) buffer);    //å°†å­—ç¬¦ä¸²è§£æžæˆjsonç»“æž„ä½“
	if (NULL == root_json) {
		cJSON_Delete(root_json);
		return;
	}

	cJSON *envEvent_rows = cJSON_GetObjectItem(root_json, "envEvent_rows");
	cJSON *event_list = envEvent_rows->child;//å­å¯¹è±¡
	if (event_list != NULL) {

		uint8_t *eventName = (uint8_t *) cJSON_GetObjectItem(event_list, "eventName")->valuestring;
		int devCnt = cJSON_GetObjectItem(event_list, "devCnt")->valueint;
		int eventId = cJSON_GetObjectItem(event_list, "eventId")->valueint;
		int valid = cJSON_GetObjectItem(event_list, "valid")->valueint;
		int thType = cJSON_GetObjectItem(event_list, "thType")->valueint;
		int envType = cJSON_GetObjectItem(event_list, "envType")->valueint;
		int valTh = cJSON_GetObjectItem(event_list, "valTh")->valueint;
		int rev = cJSON_GetObjectItem(event_list, "rev")->valueint;

		cJSON *run_dev_item = cJSON_GetObjectItem(event_list, "run_dev_item");

		int data_len = sizeof(TIMER_EVENT);
		uint8_t data_buf[data_len];
		memset(data_buf, 0, data_len);

		ENV_EVENT env_event;
		memset(env_event.eventName, 0, NAME_LEN_BYTE);
		string_to_bytes(eventName, env_event.eventName, strlen(eventName));
		uint8_t *uid = (uint8_t *) cJSON_GetObjectItem(event_list, "uidSrc")->valuestring;
		string_to_bytes(uid, env_event.uidSrc, 24);

		env_event.valTh = valTh;
		env_event.thType = thType;
		env_event.envType = envType;
		env_event.devCnt = devCnt;
		env_event.eventId = eventId;
		env_event.valid = valid;
		env_event.rev = rev;

		if (devCnt > 0) {
			cJSON *tasklist = run_dev_item->child;//å­å¯¹è±¡
			int j = 0;
			while (tasklist != NULL) {
				uint8_t *uid = (uint8_t *) cJSON_GetObjectItem(tasklist, "canCpuID")->valuestring;
				string_to_bytes(uid, env_event.itemAry[j].uid, 24);
				int devType = cJSON_GetObjectItem(tasklist, "devType")->valueint;
				env_event.itemAry[j].devType = devType;
				int devID = cJSON_GetObjectItem(tasklist, "devID")->valueint;
				env_event.itemAry[j].devID = devID;
				int lmVal = cJSON_GetObjectItem(tasklist, "lmVal")->valueint;
				env_event.itemAry[j].lmVal = lmVal;
				env_event.itemAry[j].rev2 = 0;
				env_event.itemAry[j].rev3 = 0;
				int bOnOff = cJSON_GetObjectItem(tasklist, "bOnOff")->valueint;
				env_event.itemAry[j].bOnoff = bOnOff;
				env_event.itemAry[j].param1 = 0;
				env_event.itemAry[j].param2 = 0;

				tasklist = tasklist->next;
				j++;
			}
		}

		memcpy(data_buf, &env_event, sizeof(ENV_EVENT));
		int len = UDP_PKT_SIZE + data_len;
		UDPPROPKT *send_pkt = pre_send_udp_pkt(data_buf,
											   data_len, e_udpPro_editEnvEvents, devUnitID, IS_ACK,
											   0, 0);
		send_data_to_gw(devUnitID, send_pkt, len);
	}
}

void get_secs_trigger_pkt(uint8_t *devUnitID) {
	int len = UDP_PKT_SIZE;
	UDPPROPKT *send_pkt = pre_send_udp_pkt(0, 0, e_udpPro_secs_trigger, devUnitID, IS_ACK, 0, 255);

	send_data_to_gw(devUnitID, send_pkt, len);
}

void set_secs_trigger_pkt(uint8_t *devUnitID, int subType1, int subType2, char *buffer) {

	int data_len = sizeof(SECS_TRIGGER);
	uint8_t data_buf[data_len];
	memset(data_buf, 0, data_len);

	if (subType1 == 2) {
		cJSON *root_json = cJSON_Parse((char *) buffer);    //å°†å­—ç¬¦ä¸²è§£æžæˆjsonç»“æž„ä½“
		if (NULL == root_json) {
			printf("error:%s\n", cJSON_GetErrorPtr());
			cJSON_Delete(root_json);
			return;
		}
		cJSON *secs_trigger_rows = cJSON_GetObjectItem(root_json, "secs_trigger_rows");
		cJSON *event_list = secs_trigger_rows->child;//å­å¯¹è±¡
		if (event_list != NULL) {

			uint8_t *triggerName = (uint8_t *) cJSON_GetObjectItem(event_list, "triggerName")->valuestring;
			int triggerSecs = cJSON_GetObjectItem(event_list, "triggerSecs")->valueint;
			int triggerId = cJSON_GetObjectItem(event_list, "triggerId")->valueint;
			int valid = cJSON_GetObjectItem(event_list, "valid")->valueint;
			int reportServ = cJSON_GetObjectItem(event_list, "reportServ")->valueint;
			int itemCnt = cJSON_GetObjectItem(event_list, "devCnt")->valueint;

			cJSON *itemAry = cJSON_GetObjectItem(event_list, "run_dev_item");

			SECS_TRIGGER trigger;
			memset(trigger.triggerName, 0, NAME_LEN_BYTE);
			string_to_bytes(triggerName, trigger.triggerName, 24);
			trigger.triggerSecs = triggerSecs;
			trigger.triggerId = triggerId;
			trigger.valid = valid;
			trigger.itemCnt = itemCnt;
			trigger.reportServ = reportServ;
			trigger.itemCnt = itemCnt;

			if (itemCnt > 0) {
				cJSON *tasklist = itemAry->child;//å­å¯¹è±¡
				int j = 0;
				while (tasklist != NULL) {
					uint8_t *uid = (uint8_t *) cJSON_GetObjectItem(tasklist, "canCpuID")->valuestring;
					string_to_bytes(uid, trigger.itemAry[j].uid, 24);
					int devType = cJSON_GetObjectItem(tasklist, "devType")->valueint;
					trigger.itemAry[j].devType = devType;
					int devID = cJSON_GetObjectItem(tasklist, "devID")->valueint;
					trigger.itemAry[j].devID = devID;
					int lmVal = cJSON_GetObjectItem(tasklist, "lmVal")->valueint;
					trigger.itemAry[j].lmVal = lmVal;
					trigger.itemAry[j].rev2 = 0;
					trigger.itemAry[j].rev3 = 0;
					int bOnOff = cJSON_GetObjectItem(tasklist, "bOnOff")->valueint;
					trigger.itemAry[j].bOnoff = bOnOff;
					trigger.itemAry[j].param1 = 0;
					trigger.itemAry[j].param2 = 0;

					tasklist = tasklist->next;
					j++;
				}
			}
			memcpy(data_buf, &trigger, sizeof(SECS_TRIGGER));
		}
		int len = UDP_PKT_SIZE + data_len;
		UDPPROPKT *send_pkt = pre_send_udp_pkt(data_buf, data_len, e_udpPro_secs_trigger, devUnitID, IS_ACK,
											   subType1, subType2);
		send_data_to_gw(devUnitID, send_pkt, len);
	}
}

static void extract_json(uint8_t *buffer) {

	uint8_t devUnitID[ID_LEN_BYTE];
	uint8_t devUnitPass[PASS_LEN_STR];
	int devType;
	int devID;

	//è§£æžJSONæ•°æ®
	cJSON *root_json = cJSON_Parse((char *) buffer);    //å°†å­—ç¬¦ä¸²è§£æžæˆjsonç»“æž„ä½“
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
		case e_udpPro_getBroadCast:
			break;
		case e_udpPro_getRcuInfoNoPwd: {
			memset(devUnitID, 0xff, 12);
			create_udp_broadcast(devUnitID);
		}
			break;
		case e_udpPro_getRcuInfo: {
			get_rcu_pkt(devUnitID);
		}
			break;
		case e_udpPro_setRcuInfo: {
			uint8_t name[12];
			uint8_t IpAddr[4];
			uint8_t SubMask[4];
			uint8_t Gateway[4];
			uint8_t roomNum[4];
			uint8_t macAddr[6];
			uint8_t centerServ[4];

			memset(name, 0, 12);
			memset(IpAddr, 0, 4);
			memset(Gateway, 0, 4);
			memset(SubMask, 0, 4);
			memset(roomNum, 0, 4);
			memset(macAddr, 0, 6);
			memset(centerServ, 0, 4);

			char *name_str = cJSON_GetObjectItem(root_json, "name")->valuestring;
			if (name_str != NULL) {
				memcpy(name, name_str, 12);
				free(name_str);
			}
			char *ip_str = cJSON_GetObjectItem(root_json, "IpAddr")->valuestring;
			if (ip_str != NULL) {
				const char *div = ".";
				char *p;
				p = strtok(ip_str, div);
				int i = 0;
				while (p) {
					IpAddr[i] = atoi(p);
					i++;
					p = strtok(NULL, div);
				}
				free(ip_str);
			}

			char *subMask_str = cJSON_GetObjectItem(root_json, "SubMask")->valuestring;
			if (subMask_str != NULL) {
				const char *div = ".";
				char *p;
				p = strtok(subMask_str, div);
				int i = 0;
				while (p) {
					SubMask[i] = atoi(p);
					i++;
					p = strtok(NULL, div);
				}
				free(subMask_str);
			}

			char *gateway_str = cJSON_GetObjectItem(root_json, "Gateway")->valuestring;
			if (gateway_str != NULL) {
				const char *div = ".";
				char *p;
				p = strtok(gateway_str, div);
				int i = 0;
				while (p) {
					Gateway[i] = atoi(p);
					i++;
					p = strtok(NULL, div);
				}
				free(gateway_str);
			}

			char *centerServ_str = cJSON_GetObjectItem(root_json,
													   "centerServ")->valuestring;
			if (centerServ_str != NULL) {
				const char *div = ".";
				char *p;
				p = strtok(centerServ_str, div);
				int i = 0;
				while (p) {
					centerServ[i] = atoi(p);
					i++;
					p = strtok(NULL, div);
				}
				free(centerServ_str);
			}

			char *roomNum_str = cJSON_GetObjectItem(root_json, "roomNum")->valuestring;
			if (roomNum_str != NULL) {
				memcpy(roomNum_str, roomNum, 4);
				free(roomNum_str);
			}

			char *macAddr_str = cJSON_GetObjectItem(root_json, "macAddr")->valuestring;
			if (macAddr_str != NULL) {
				string_to_bytes(macAddr_str, macAddr, 12);
				free(macAddr_str);
			}

			int bDhcp = cJSON_GetObjectItem(root_json, "bDhcp")->valueint;

			set_rcuInfo_pkt(devUnitID, devUnitPass, name, IpAddr, SubMask, Gateway,
							centerServ,
							roomNum, macAddr, bDhcp);
		}
			break;
		case e_udpPro_getDevsInfo: get_devs_pkt(devUnitID);
			break;
		case e_udpPro_addDev: {
			devType = cJSON_GetObjectItem(root_json, "devType")->valueint;
			char *devName_str = cJSON_GetObjectItem(root_json, "devName")->valuestring;
			char *roomName_str = cJSON_GetObjectItem(root_json, "roomName")->valuestring;

			int powChn = cJSON_GetObjectItem(root_json, "powChn")->valueint;
			char *canCpuID_str = cJSON_GetObjectItem(root_json, "canCpuID")->valuestring;

			ctrl_devs_pkt(devUnitID, canCpuID_str, e_udpPro_addDev, devType, 0, powChn,
						  devName_str, roomName_str, 0, buffer);
		}
			break;
		case e_udpPro_editDev: {
			//é‡æ–°æ‰“åŒ…æ•°æ®ï¼Œè½¬å‘ç»™è”ç½‘æ¨¡å—
			devType = cJSON_GetObjectItem(root_json, "devType")->valueint;
			devID = cJSON_GetObjectItem(root_json, "devID")->valueint;
			//æŽ§åˆ¶æ—¶cmdä¸ºæŽ§åˆ¶å‘½ä»¤ï¼Œç¼–è¾‘å’Œåˆ é™¤æ—¶cmdä¸ºè®¾å¤‡ç±»åž‹T_WARE_TYPE
			int cmd = cJSON_GetObjectItem(root_json, "cmd")->valueint;
			char *canCpuID_str = cJSON_GetObjectItem(root_json, "canCpuID")->valuestring;
			char *devName_str = cJSON_GetObjectItem(root_json, "devName")->valuestring;
			char *roomName_str = cJSON_GetObjectItem(root_json, "roomName")->valuestring;

			int powChn = cJSON_GetObjectItem(root_json, "powChn")->valueint;
			ctrl_devs_pkt(devUnitID, canCpuID_str, e_udpPro_editDev, devType, devID,
						  powChn, devName_str, roomName_str, cmd, buffer);
		}
			break;
		case e_udpPro_delDev: {
			//é‡æ–°æ‰“åŒ…æ•°æ®ï¼Œè½¬å‘ç»™è”ç½‘æ¨¡å—
			devType = cJSON_GetObjectItem(root_json, "devType")->valueint;
			devID = cJSON_GetObjectItem(root_json, "devID")->valueint;
			//æŽ§åˆ¶æ—¶cmdä¸ºæŽ§åˆ¶å‘½ä»¤ï¼Œç¼–è¾‘å’Œåˆ é™¤æ—¶cmdä¸ºè®¾å¤‡ç±»åž‹T_WARE_TYPE
			int cmd = cJSON_GetObjectItem(root_json, "cmd")->valueint;

			char *canCpuID_str = cJSON_GetObjectItem(root_json, "canCpuID")->valuestring;
			ctrl_devs_pkt(devUnitID, canCpuID_str, e_udpPro_delDev, devType, devID, 0, 0,
						  0, cmd, buffer);
		}
			break;
		case e_udpPro_ctrlDev: {//é‡æ–°æ‰“åŒ…æ•°æ®ï¼Œè½¬å‘ç»™è”ç½‘æ¨¡å—
			devType = cJSON_GetObjectItem(root_json, "devType")->valueint;
			devID = cJSON_GetObjectItem(root_json, "devID")->valueint;
			//æŽ§åˆ¶æ—¶cmdä¸ºæŽ§åˆ¶å‘½ä»¤ï¼Œç¼–è¾‘å’Œåˆ é™¤æ—¶cmdä¸ºè®¾å¤‡ç±»åž‹T_WARE_TYPE
			int cmd = cJSON_GetObjectItem(root_json, "cmd")->valueint;

			char *canCpuID_str = cJSON_GetObjectItem(root_json, "canCpuID")->valuestring;
			ctrl_devs_pkt(devUnitID, canCpuID_str, e_udpPro_ctrlDev, devType, devID, 0, 0,
						  0, cmd, buffer);
		}
			break;
		case e_udpPro_getBoards: {
			get_board_pkt(devUnitID);
		}
			break;
		case e_udpPro_editBoards: {
			cJSON *item = cJSON_GetObjectItem(root_json, "keyinput_rows");
			edit_broad_json(devUnitID, item);
		}
		case e_udpPro_getSceneEvents: {
			get_scenes_pkt(devUnitID);
		}
			break;
		case e_udpPro_addSceneEvents:
		case e_udpPro_editSceneEvents:
		case e_udpPro_delSceneEvents: {
			char *sceneName = cJSON_GetObjectItem(root_json,
												  "sceneName")->valuestring;
			int devCnt = cJSON_GetObjectItem(root_json, "devCnt")->valueint;
			int eventId = cJSON_GetObjectItem(root_json, "eventId")->valueint;
			int datType = cJSON_GetObjectItem(root_json, "datType")->valueint;
			cJSON *itemAry = cJSON_GetObjectItem(root_json, "itemAry");

			add_scene_pkt(devUnitID, sceneName, devCnt, eventId, itemAry,
						  datType);
		}
			break;
		case e_udpPro_exeSceneEvents: {
			int eventId = cJSON_GetObjectItem(root_json, "eventId")->valueint;

			ctrl_scene_pkt(devUnitID, eventId, datType);
		}
			break;
		case e_udpPro_getKeyOpItems: {
			char *canCpuID_str = cJSON_GetObjectItem(root_json, "canCpuID")->valuestring;
			uint8_t uid[12];
			string_to_bytes(canCpuID_str, uid, 24);

			int key_index = cJSON_GetObjectItem(root_json, "key_index")->valueint;
			get_keyOpitem_pkt(devUnitID, key_index, canCpuID_str);
		}
			break;
		case e_udpPro_setKeyOpItems: {

			char *cpuCanID_str = cJSON_GetObjectItem(root_json,
													 "key_cpuCanID")->valuestring;
			uint8_t canCpuID[12];
			string_to_bytes(cpuCanID_str, canCpuID, 24);

			int key_index = cJSON_GetObjectItem(root_json, "key_index")->valueint;
			int cnt = cJSON_GetObjectItem(root_json, "key_opitem")->valueint;
			cJSON *keyop_item = cJSON_GetObjectItem(root_json, "key_opitem_rows");

			set_key_opitem_pkt(devUnitID, canCpuID, key_index, cnt, keyop_item,
							   e_udpPro_setKeyOpItems);
		}
			break;
		case e_udpPro_getChnOpItems: {
			char *cpuCanID_str = cJSON_GetObjectItem(root_json, "canCpuID")->valuestring;
			uint8_t canCpuID[12];
			string_to_bytes(cpuCanID_str, canCpuID, ID_LEN_BYTE * 2);

			devType = cJSON_GetObjectItem(root_json, "devType")->valueint;
			devID = cJSON_GetObjectItem(root_json, "devID")->valueint;

			get_chnOpitem_pkt(devUnitID, devType, devID, canCpuID);
		}
			break;
		case e_udpPro_setChnOpItems: {
			char *cpuCanID_str = cJSON_GetObjectItem(root_json,
													 "out_cpuCanID")->valuestring;
			uint8_t canCpuID[12];
			string_to_bytes(cpuCanID_str, canCpuID, ID_LEN_BYTE * 2);

			devType = cJSON_GetObjectItem(root_json, "devType")->valueint;
			devID = cJSON_GetObjectItem(root_json, "devID")->valueint;
			int cnt = cJSON_GetObjectItem(root_json, "chn_opitem")->valueint;
			cJSON *chnop_item = cJSON_GetObjectItem(root_json, "chn_opitem_rows");

			set_chn_opitem_pkt(devUnitID, canCpuID, devType, devID, cnt, chnop_item,
							   e_udpPro_setChnOpItems);
		}
			break;
		case e_udpPro_security_info: {
			int subType1 = cJSON_GetObjectItem(root_json, "subType1")->valueint;
			int subType2 = cJSON_GetObjectItem(root_json, "subType2")->valueint;
			if (subType1 == 0 || subType1 == 3) {
				get_security_pkt(devUnitID, subType1, subType2);
			} else {
				set_security_pkt(devUnitID, subType1, subType2, buffer);
			}
		}
			break;
		case e_udpPro_get_key2scene: {
			get_key2scene_pkt(devUnitID);
		}
			break;
		case e_udpPro_set_key2scene: {
			cJSON *itemAry = cJSON_GetObjectItem(root_json, "key2scene_item");
			int cnt = cJSON_GetObjectItem(root_json, "itemCnt")->valueint;
			set_key2scene_pkt(devUnitID, cnt, itemAry);
		}
			break;
		case e_udpPro_getTimerEvents: {
			get_timerEvent_pkt(devUnitID);
		}
			break;
		case e_udpPro_addTimerEvents:
		case e_udpPro_editTimerEvents: {
			set_timerEvent_pkt(devUnitID, buffer);
		}
			break;
		case e_udpPro_getEnvEvents: {
			get_EnvEvents_pkt(devUnitID);
		}
			break;
		case e_udpPro_editEnvEvents: {
			set_envEvent_pkt(devUnitID, buffer);
		}
			break;
		case e_udpPro_secs_trigger: {
			int subType1 = cJSON_GetObjectItem(root_json, "subType1")->valueint;
			int subType2 = cJSON_GetObjectItem(root_json, "subType2")->valueint;
			if (subType1 == 0) {
				get_secs_trigger_pkt(devUnitID);
			} else {
				set_secs_trigger_pkt(devUnitID, subType1, subType2, buffer);
			}
		}
			break;
		default:break;
	}

	cJSON_Delete(root_json);
}

