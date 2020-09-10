#include <stdio.h>
#include "format_bin_to_json.h"
#include "../comm/str_comm.h"
#include "../json/cJSON.h"
#include "../json/s2j.h"

typedef struct {
  char *devUnitID;
  char *devPass;
  int datType;
  int subType1;
  int subType2;
  int data_len;
  void *data;
} JsonBase;

typedef struct jsonContext {
  void *data;
  cJSON *(*processor)(struct jsonContext *p, cJSON *root);
} JsonContext;

static char *create_json(struct jsonContext *pThis) {
	cJSON *root;
	root = cJSON_CreateObject();

	if (root == NULL) {
		return NULL;
	}

	//变化处理方法
	root = pThis->processor(pThis, root);

	//char *p = cJSON_Print(root);
	char *p = cJSON_PrintUnformatted(root);
	if (NULL == p) {
		cJSON_Delete(root);
		return NULL;
	}

	cJSON_Delete(root);

	return p;
}

static cJSON *struct_base_to_json(void *struct_obj) {
	JsonBase *base = (JsonBase *) struct_obj;

	s2j_create_json_obj(json_base);

	s2j_json_set_basic_element(json_base, base, string, devUnitID);
	s2j_json_set_basic_element(json_base, base, int, datType);
	s2j_json_set_basic_element(json_base, base, int, subType1);
	s2j_json_set_basic_element(json_base, base, int, subType2);

	return json_base;
}

static cJSON *create_base_json(JsonBase *p, cJSON *root) {
	root = struct_base_to_json(p);
	return root;
}

typedef struct {
  char *uid;
  char *pass;
  char *name;
  char *IpAddr;
  char *SubMask;
  char *Gateway;
  char *centerServ;
  char *roomNum;
  char *macAddr;
  char *SoftVersion;
  char *HwVersion;
  int bDhcp;
} RCU_ROWS;

typedef struct {
  JsonBase base;
  RCU_ROWS rcu_rows;
} RCU;

static void convert_addr(char addr[], uint8_t *buf) {
	sprintf(addr, "%d.%d.%d.%d", buf[0] & 0xff, buf[1] & 0xff, buf[2] & 0xff, buf[3] & 0xff);
}

static cJSON *struct_rcu_to_json(void *struct_obj) {
	RCU *rcu = (RCU *) struct_obj;

	s2j_create_json_obj(root);
	root = create_base_json(&rcu->base, root);

	s2j_json_set_struct_element(json_rcu, root, struct_rcu_rows, rcu, RCU_ROWS, rcu_rows, 1);
	s2j_create_json_obj(item);
	s2j_json_set_basic_element(item, struct_rcu_rows, string, uid);
	s2j_json_set_basic_element(item, struct_rcu_rows, string, pass);
	s2j_json_set_basic_element(item, struct_rcu_rows, string, name);
	s2j_json_set_basic_element(item, struct_rcu_rows, string, IpAddr);
	s2j_json_set_basic_element(item, struct_rcu_rows, string, SubMask);
	s2j_json_set_basic_element(item, struct_rcu_rows, string, Gateway);
	s2j_json_set_basic_element(item, struct_rcu_rows, string, centerServ);
	s2j_json_set_basic_element(item, struct_rcu_rows, string, roomNum);
	s2j_json_set_basic_element(item, struct_rcu_rows, string, macAddr);
	s2j_json_set_basic_element(item, struct_rcu_rows, string, SoftVersion);
	s2j_json_set_basic_element(item, struct_rcu_rows, string, HwVersion);
	s2j_json_set_basic_element(item, struct_rcu_rows, int, bDhcp);

	s2j_json_array_set_object_element(json_rcu, item);

	return root;
}

static cJSON *create_rcu(struct jsonContext *p, cJSON *root) {
	JsonBase *base = (JsonBase *) p->data;

	RCU_INFO *rcu_info = (RCU_INFO *) base->data;
	char devUnitID[25] = {0};
	bytes_to_string(rcu_info->devUnitID, (uint8_t *) devUnitID, 12);

	char pass[9] = {0};
	substring(pass, devUnitID, 16, 24);

	char ipaddr[16];
	char submask[16];
	char gateway[16];
	char centrserv[16];
	char name[25] = {0};
	char macaddr[13] = {0};

	if (strlen(rcu_info->name) > 12)
		bytes_to_string(rcu_info->name, name, 12);
	else
		bytes_to_string(rcu_info->name, name, strlen(rcu_info->name));

	convert_addr(ipaddr, rcu_info->IpAddr);
	convert_addr(submask, rcu_info->SubMask);
	convert_addr(gateway, rcu_info->Gateway);
	convert_addr(centrserv, rcu_info->centerServ);

	bytes_to_string(rcu_info->macAddr, macaddr, 6);
	RCU_ROWS rcu_rows = {devUnitID, pass, name, ipaddr, submask, gateway, centrserv, "", macaddr, "", "", 0};
	RCU rcu = {*base, rcu_rows};
	root = struct_rcu_to_json(&rcu);

	return root;
}

char *create_rcu_json(UDPPROPKT *pkt) {
	char uid[25] = {0};
	bytes_to_string(pkt->uidSrc, (uint8_t *) uid, 12);

	char pass[9] = {0};
	memcpy(pass, uid, 8);

	JsonBase base = {uid, pass, pkt->datType, pkt->subType1, pkt->subType2, pkt->datLen, pkt->dat};

	JsonContext jxt = {&base, create_rcu};
	char *str = create_json(&jxt);
	return str;
}

typedef struct {
  int devState;          //输出模块的通道状态
  char *devUnitID;  //输出模块的cpuID
} StateRows;

typedef struct {
  JsonBase base;
  StateRows state_rows;
} ChnStates;

static cJSON *struct_state_to_json(void *struct_obj) {
	ChnStates *chnStates = (ChnStates *) struct_obj;

	s2j_create_json_obj(root);
	root = create_base_json(&chnStates->base, root);
	s2j_json_set_struct_element(json_state, root, struct_state_rows, chnStates, StateRows, state_rows, 1);

	s2j_create_json_obj(item);
	s2j_json_set_child_struct_element(item, struct_state_rows, int, devState);
	s2j_json_set_child_struct_element(item, struct_state_rows, string, devUnitID);
	s2j_json_array_set_object_element(json_state, item);

	return root;
}

static cJSON *create_chn_status(JsonContext *p, cJSON *root) {
	JsonBase *base = (JsonBase *) p->data;

	CHNS_STATUS *status = (CHNS_STATUS *) base->data;
	char bin_string[13];
	itoa_bin(status->state, bin_string);

	char uid[25] = {0};
	bytes_to_string(status->devUnitID, (uint8_t *) uid, 12);

	StateRows state_rows = {status->state, uid};
	ChnStates chnStates = {*base, state_rows};

	root = struct_state_to_json(&chnStates);
	return root;
}

char *create_chn_status_json(UDPPROPKT *pkt) {
	char uid[25] = {0};
	bytes_to_string(pkt->uidSrc, (uint8_t *) uid, 12);

	char pass[9] = {0};
	memcpy(pass, uid, 8);

	JsonBase base = {uid, pass, pkt->datType, pkt->subType1, pkt->subType2, pkt->datLen, pkt->dat};

	JsonContext jxt = {&base, create_chn_status};
	char *str = create_json(&jxt);
	return str;
}

typedef struct {
  char *canCpuID;
  int devID;
  int devType;
  int lmVal;
  int rev2;
  int rev3;
  int bOnOff;
  int param1;
  int param2;
} RUN_DEV_ITEM_JSON;

typedef struct {
  char *sceneName;
  int devCnt;
  int eventId;
  int exeSecu;
  RUN_DEV_ITEM_JSON run_dev_item[32];
} SceneRows;

typedef struct {
  JsonBase base;
  SceneRows scene_rows;
  int scene;
} Scene;

static cJSON *struct_run_dev_item_to_json(void *struct_obj) {
	RUN_DEV_ITEM_JSON *item = (RUN_DEV_ITEM_JSON *) struct_obj;

	s2j_create_json_obj(json_item);
	s2j_json_set_child_struct_element(json_item, item, string, canCpuID);
	s2j_json_set_child_struct_element(json_item, item, int, devID);
	s2j_json_set_child_struct_element(json_item, item, int, devType);
	s2j_json_set_child_struct_element(json_item, item, int, rev2);
	s2j_json_set_child_struct_element(json_item, item, int, rev3);
	s2j_json_set_child_struct_element(json_item, item, int, bOnOff);
	s2j_json_set_child_struct_element(json_item, item, int, param1);
	s2j_json_set_child_struct_element(json_item, item, int, param2);

	return json_item;
}

static cJSON *struct_scene_to_json(void *struct_obj) {
	Scene *scene = (Scene *) struct_obj;

	s2j_create_json_obj(root);
	root = create_base_json(&scene->base, root);
	s2j_json_set_struct_element(json_scene, root, struct_scene_rows, scene, SceneRows, scene_rows, 1);
	s2j_json_set_basic_element(root, scene, int, scene);

	s2j_create_json_obj(item);
	s2j_json_set_basic_element(item, struct_scene_rows, string, sceneName);
	s2j_json_set_basic_element(item, struct_scene_rows, int, devCnt);
	s2j_json_set_basic_element(item, struct_scene_rows, int, eventId);
	s2j_json_set_basic_element(item, struct_scene_rows, int, exeSecu);

	s2j_json_set_struct_element(json_scene_rows, item, struct_run_dev_item, &(scene->scene_rows),
								RUN_DEV_ITEM, run_dev_item, 32);
	for (int i = 0; i < scene->scene_rows.devCnt; i++) {
		cJSON *json_item = struct_run_dev_item_to_json(&scene->scene_rows.run_dev_item[i]);
		s2j_json_array_set_object_element(json_scene_rows, json_item);
	}
	s2j_json_array_set_object_element(json_scene, item);

	return root;
}

static cJSON *create_events(JsonContext *p, cJSON *root) {

	JsonBase *base = (JsonBase *) p->data;
	int event_cnt = base->data_len / sizeof(SCENE_EVENT);

	SCENE_EVENT *event = (SCENE_EVENT *) (base->data);
	if (event->eventId == 0 || event->eventId == 1) {
		return NULL;
	}

	char sceneName[25] = {0};
	if (strlen(event->sceneName) > 12) {
		bytes_to_string(event->sceneName, sceneName, 12);
	} else
		bytes_to_string(event->sceneName, sceneName, strlen(event->sceneName));

	Scene scene;
	SceneRows SceneRows = {sceneName, event->devCnt, event->eventId, event->exeSecu};

	if (base->datType == e_udpPro_getSceneEvents) {
		for (int num = 0; num < event->devCnt; num++) {
			RUN_DEV_ITEM run_dev_item = (RUN_DEV_ITEM) event->itemAry[num];

			uint8_t canCpuID[25] = {0};
			bytes_to_string(run_dev_item.uid, canCpuID, 12);
			RUN_DEV_ITEM_JSON
				item = {(char *) canCpuID, run_dev_item.devID, run_dev_item.devType,
				run_dev_item.lmVal,
				run_dev_item.rev2, run_dev_item.rev3, run_dev_item.bOnoff,
				run_dev_item.param1,
				run_dev_item.param2};
			SceneRows.run_dev_item[num] = item;
		}
		scene.scene_rows = SceneRows;
		scene.base = *base;
		scene.scene = event_cnt;

		root = struct_scene_to_json(&scene);
		return root;
	}
}

char *create_events_json(UDPPROPKT *pkt) {
	char uid[25] = {0};
	bytes_to_string(pkt->uidSrc, (uint8_t *) uid, 12);

	char pass[9] = {0};
	substring(pass, uid, 16, 24);

	JsonBase base = {uid, pass, pkt->datType, pkt->subType1, pkt->subType2, pkt->datLen, pkt->dat};

	JsonContext jxt = {&base, create_events};
	char *str = create_json(&jxt);
	return str;
}

typedef struct {
  char canCpuID[25];
  char boardName[25];
  int boardType;
  int chnCnt;
  int bOnline;
} board_struct;

static cJSON *struct_board_to_json(void *struct_obj) {
	board_struct *board = (board_struct *) struct_obj;

	s2j_create_json_obj(json_board);

	s2j_json_set_basic_element(json_board, board, string, canCpuID);
	s2j_json_set_basic_element(json_board, board, string, boardName);
	s2j_json_set_basic_element(json_board, board, int, boardType);
	s2j_json_set_basic_element(json_board, board, int, chnCnt);
	s2j_json_set_basic_element(json_board, board, int, bOnline);

	return json_board;
}

static cJSON *create_board_chnout(JsonContext *p, cJSON *root) {
	cJSON *chnout_rows;

	JsonBase *base = (JsonBase *) p->data;
	root = create_base_json(base, root);
	cJSON_AddItemToObject(root, "chnout_rows", chnout_rows = cJSON_CreateArray());

	int board_cnt = base->data_len / sizeof(BOARD_CHNOUT);

	for (int i = 0; i < board_cnt; i++) {
		BOARD_CHNOUT *board = (BOARD_CHNOUT *) (p->data + i * sizeof(BOARD_CHNOUT));;

		uint32_t canCpuID[25] = {0};
		bytes_to_string(board->devUnitID, canCpuID, 12);

		char boardName[25] = {0};
		if (strlen(board->boardName) > 12) {
			bytes_to_string(board->boardName, boardName, 12);
		} else {
			bytes_to_string(board->boardName, boardName, strlen(board->boardName));
		}

		board_struct board_uct = {(char *) canCpuID, boardName, board->boardType, board->chnCnt,
			board->bOnline};
		cJSON *item = struct_board_to_json(&board_uct);

		cJSON *chnName_rows = NULL;
		cJSON_AddItemToObject(item, "chnName_rows", chnName_rows = cJSON_CreateArray());
		for (int j = 0; j < board->chnCnt; j++) {
			char chnName[25] = {0};
			if (strlen(board->chnName[j]) > 12) {
				bytes_to_string(board->chnName[j], chnName, 12);
			} else
				bytes_to_string(board->chnName[j], chnName, strlen(board->chnName[j]));

			cJSON_AddItemToObject(chnName_rows, "chnName", cJSON_CreateString(chnName));
		}
		cJSON_AddItemToArray(chnout_rows, item);
	}

	cJSON_AddItemToObject(root, "board", cJSON_CreateNumber(board_cnt));

	return root;
}

char *create_board_chnout_json(UDPPROPKT *pkt) {
	char uid[25] = {0};
	bytes_to_string(pkt->uidSrc, (uint32_t *) uid, 12);

	char pass[9] = {0};
	substring(pass, uid, 16, 24);

	JsonBase base = {uid, pass, pkt->datType, pkt->subType1, pkt->subType2, pkt->datLen, pkt->dat};
	JsonContext jxt = {&base, create_board_chnout};

	char *str = create_json(&jxt);
	return str;
}

typedef struct {
  char *canCpuID;
  char *boardName;
  int boardType;
  int keyCnt;
  int ledBkType;
  char *roomName;
} keyinput_struct;

static cJSON *struct_keyinput_to_json(void *struct_obj) {
	keyinput_struct *board = (keyinput_struct *) struct_obj;

	s2j_create_json_obj(json_keyinput);

	s2j_json_set_basic_element(json_keyinput, board, string, canCpuID);
	s2j_json_set_basic_element(json_keyinput, board, string, boardName);
	s2j_json_set_basic_element(json_keyinput, board, int, boardType);
	s2j_json_set_basic_element(json_keyinput, board, int, keyCnt);
	s2j_json_set_basic_element(json_keyinput, board, int, ledBkType);
	s2j_json_set_basic_element(json_keyinput, board, string, roomName);

	return json_keyinput;
}

static cJSON *create_board_keyinput(JsonContext *p, cJSON *root) {
	cJSON *keyinput_rows, *keyName_rows, *keyAllCtrlType_rows;

	JsonBase *base = (JsonBase *) p->data;
	root = create_base_json(base, root);

	cJSON_AddItemToObject(root, "keyinput_rows", keyinput_rows = cJSON_CreateArray());

	int keyinput_cnt = base->data_len / sizeof(BOARD_KEYINPUT);

	for (int i = 0; i < keyinput_cnt; i++) {
		BOARD_KEYINPUT *keyinput = (BOARD_KEYINPUT *) (p->data + i * sizeof(BOARD_KEYINPUT));

		uint32_t canCpuID[25] = {0};
		bytes_to_string(keyinput->devUnitID, canCpuID, 12);

		char boardName[25] = {0};
		if (strlen(keyinput->boardName) > 12) {
			bytes_to_string(keyinput->boardName, boardName, 12);
		} else {
			bytes_to_string(keyinput->boardName, boardName, strlen(keyinput->boardName));
		}
		char roomName[25] = {0};
		if (strlen(keyinput->roomName) > 12) {
			bytes_to_string(keyinput->roomName, boardName, 12);
		} else
			bytes_to_string(keyinput->roomName, roomName, strlen(keyinput->roomName));

		keyinput_struct keyinput_uct =
			{(char *) canCpuID, boardName, keyinput->boardType, keyinput->keyCnt,
				keyinput->ledBkType, roomName};
		cJSON *item = struct_keyinput_to_json(&keyinput_uct);

		cJSON_AddItemToObject(item, "keyName_rows", keyName_rows = cJSON_CreateArray());
		for (int j = 0; j < 6; j++) {
			char keyName[25] = {0};
			if (strlen(keyinput->keyName[j]) > 12) {
				bytes_to_string(keyinput->keyName[j], keyName, 12);
			} else {
				bytes_to_string(keyinput->keyName[j], keyName, strlen(keyinput->keyName[j]));
			}
			cJSON_AddItemToObject(keyName_rows, "keyName", cJSON_CreateString(keyName));
		}

		cJSON_AddItemToObject(item, "keyAllCtrlType_rows",
							  keyAllCtrlType_rows = cJSON_CreateArray());
		for (int k = 0; k < 6; k++) {
			cJSON_AddItemToObject(keyAllCtrlType_rows, "keyAllCtrlType",
								  cJSON_CreateNumber(keyinput->keyAllCtrlType[k]));
		}

		cJSON_AddItemToArray(keyinput_rows, item);

	}

	cJSON_AddItemToObject(root, "keyinput", cJSON_CreateNumber(keyinput_cnt));

	return root;

}

char *create_board_keyinput_json(UDPPROPKT *pkt) {
	char uid[25] = {0};
	bytes_to_string(pkt->uidSrc, (uint32_t *) uid, 12);

	char pass[9] = {0};
	memcpy(pass, uid, 8);

	JsonBase base = {uid, pass, pkt->datType, pkt->subType1, pkt->subType2, pkt->datLen, pkt->dat};
	JsonContext jxt = {&base, create_board_keyinput};

	char *str = create_json(&jxt);
	return str;
}

typedef struct {
  char *canCpuID;
  char *devName;
  char *roomName;
  int devType;
  int devID;
} waredev_struct;

static cJSON *struct_waredev_to_json(void *struct_obj) {
	waredev_struct *ware_dev = (waredev_struct *) struct_obj;

	s2j_create_json_obj(json_ware_dev);

	s2j_json_set_basic_element(json_ware_dev, ware_dev, string, canCpuID);
	s2j_json_set_basic_element(json_ware_dev, ware_dev, string, devName);
	s2j_json_set_basic_element(json_ware_dev, ware_dev, string, roomName);
	s2j_json_set_basic_element(json_ware_dev, ware_dev, int, devType);
	s2j_json_set_basic_element(json_ware_dev, ware_dev, int, devID);

	return json_ware_dev;
}

typedef struct {
  char *canCpuID;
  char *devName;
  char *roomName;
  int devType;
  int devID;
  int bOnOff;
  int bTuneEn;
  int lmVal;
  int powChn;
} light_struct;

static cJSON *struct_light_to_json(void *struct_obj) {
	light_struct *light = (light_struct *) struct_obj;

	s2j_create_json_obj(json_light);

	s2j_json_set_basic_element(json_light, light, string, canCpuID);
	s2j_json_set_basic_element(json_light, light, string, devName);
	s2j_json_set_basic_element(json_light, light, string, roomName);
	s2j_json_set_basic_element(json_light, light, int, devType);
	s2j_json_set_basic_element(json_light, light, int, devID);
	s2j_json_set_basic_element(json_light, light, int, bOnOff);
	s2j_json_set_basic_element(json_light, light, int, bTuneEn);
	s2j_json_set_basic_element(json_light, light, int, lmVal);
	s2j_json_set_basic_element(json_light, light, int, powChn);

	return json_light;
}

typedef struct {
  char *canCpuID;
  char *devName;
  char *roomName;
  int devType;
  int devID;
  int bOnOff;
  int selMode;
  int selTemp;
  int selSpd;
  int selDirect;
  int rev1;
  int powChn;
} aircond_struct;

static cJSON *struct_air_to_json(void *struct_obj) {
	aircond_struct *air = (aircond_struct *) struct_obj;

	s2j_create_json_obj(json_air);

	s2j_json_set_basic_element(json_air, air, string, canCpuID);
	s2j_json_set_basic_element(json_air, air, string, devName);
	s2j_json_set_basic_element(json_air, air, string, roomName);
	s2j_json_set_basic_element(json_air, air, int, devType);
	s2j_json_set_basic_element(json_air, air, int, devID);
	s2j_json_set_basic_element(json_air, air, int, bOnOff);
	s2j_json_set_basic_element(json_air, air, int, selMode);
	s2j_json_set_basic_element(json_air, air, int, selTemp);
	s2j_json_set_basic_element(json_air, air, int, selSpd);
	s2j_json_set_basic_element(json_air, air, int, selDirect);
	s2j_json_set_basic_element(json_air, air, int, rev1);
	s2j_json_set_basic_element(json_air, air, int, powChn);

	return json_air;
}

typedef struct {
  char *canCpuID;
  char *devName;
  char *roomName;
  int devType;
  int devID;
  int bOnOff;
  int timRun;
  int powChn;
} curtain_struct;

static cJSON *struct_curtain_to_json(void *struct_obj) {
	curtain_struct *curtain = (curtain_struct *) struct_obj;

	s2j_create_json_obj(json_curtain);

	s2j_json_set_basic_element(json_curtain, curtain, string, canCpuID);
	s2j_json_set_basic_element(json_curtain, curtain, string, devName);
	s2j_json_set_basic_element(json_curtain, curtain, string, roomName);
	s2j_json_set_basic_element(json_curtain, curtain, int, devType);
	s2j_json_set_basic_element(json_curtain, curtain, int, devID);
	s2j_json_set_basic_element(json_curtain, curtain, int, bOnOff);
	s2j_json_set_basic_element(json_curtain, curtain, int, timRun);
	s2j_json_set_basic_element(json_curtain, curtain, int, powChn);

	return json_curtain;
}

typedef struct {
  char *canCpuID;
  char *devName;
  char *roomName;
  int devType;
  int devID;
  int bOnOff;
  int timRun;
  int bLockOut;
  int powChnOpen;
  char *pwd;
} lock_struct;

static cJSON *struct_lock_to_json(void *struct_obj) {
	lock_struct *lock = (lock_struct *) struct_obj;

	s2j_create_json_obj(json_lock);

	s2j_json_set_basic_element(json_lock, lock, string, canCpuID);
	s2j_json_set_basic_element(json_lock, lock, string, devName);
	s2j_json_set_basic_element(json_lock, lock, string, roomName);
	s2j_json_set_basic_element(json_lock, lock, int, devType);
	s2j_json_set_basic_element(json_lock, lock, int, devID);
	s2j_json_set_basic_element(json_lock, lock, int, bOnOff);
	s2j_json_set_basic_element(json_lock, lock, int, timRun);
	s2j_json_set_basic_element(json_lock, lock, int, bLockOut);
	s2j_json_set_basic_element(json_lock, lock, int, powChnOpen);
	s2j_json_set_basic_element(json_lock, lock, string, pwd);

	return json_lock;
}

typedef struct {
  char *canCpuID;
  char *devName;
  char *roomName;
  int devType;
  int devID;
  int bOnOff;           //开关状态 0关闭  1打开
  int timRun;           //开关电机运转的时间  超时停止转动  防止有的电机没有行程限制
  int powChnOpen;       //开阀门通道
  int powChnClose;      //开阀门通道
} valve_struct;

static cJSON *struct_valve_to_json(void *struct_obj) {
	valve_struct *valve = (valve_struct *) struct_obj;

	s2j_create_json_obj(json_valve);

	s2j_json_set_basic_element(json_valve, valve, string, canCpuID);
	s2j_json_set_basic_element(json_valve, valve, string, devName);
	s2j_json_set_basic_element(json_valve, valve, string, roomName);
	s2j_json_set_basic_element(json_valve, valve, int, devType);
	s2j_json_set_basic_element(json_valve, valve, int, devID);
	s2j_json_set_basic_element(json_valve, valve, int, bOnOff);
	s2j_json_set_basic_element(json_valve, valve, int, timRun);
	s2j_json_set_basic_element(json_valve, valve, int, powChnOpen);
	s2j_json_set_basic_element(json_valve, valve, int, powChnOpen);

	return json_valve;
}

typedef struct {
  char *canCpuID;
  char *devName;
  char *roomName;
  int devType;
  int devID;
  int bOnOff;           //开关状态 0关闭  1打开
  int spdSel;           //开关电机运转的时间  超时停止转动  防止有的电机没有行程限制
  int onOffChn;       //开阀门通道
  int spdLowChn;
  int spdMidChn;
  int spdHighChn;
  int autoRun;
  int valPm10;
  int valPm25;
} frair_struct;

static cJSON *struct_frair_to_json(void *struct_obj) {
	frair_struct *frair = (frair_struct *) struct_obj;

	s2j_create_json_obj(json_frair);

	s2j_json_set_basic_element(json_frair, frair, string, canCpuID);
	s2j_json_set_basic_element(json_frair, frair, string, devName);
	s2j_json_set_basic_element(json_frair, frair, string, roomName);
	s2j_json_set_basic_element(json_frair, frair, int, devType);
	s2j_json_set_basic_element(json_frair, frair, int, devID);
	s2j_json_set_basic_element(json_frair, frair, int, bOnOff);
	s2j_json_set_basic_element(json_frair, frair, int, spdSel);
	s2j_json_set_basic_element(json_frair, frair, int, onOffChn);
	s2j_json_set_basic_element(json_frair, frair, int, spdLowChn);
	s2j_json_set_basic_element(json_frair, frair, int, spdMidChn);
	s2j_json_set_basic_element(json_frair, frair, int, spdHighChn);
	s2j_json_set_basic_element(json_frair, frair, int, autoRun);
	s2j_json_set_basic_element(json_frair, frair, int, valPm10);
	s2j_json_set_basic_element(json_frair, frair, int, valPm25);

	return json_frair;
}

typedef struct {
  char *canCpuID;
  char *devName;
  char *roomName;
  int devType;
  int devID;
  int bOnOff;           //开关状态 0关闭  1打开
  int tempGet;
  int tempSet;
  int powChn;
  int autoRun;
} floor_heat_struct;

static cJSON *struct_floorheat_to_json(void *struct_obj) {
	floor_heat_struct *floorheat = (floor_heat_struct *) struct_obj;

	s2j_create_json_obj(json_floorheat);

	s2j_json_set_basic_element(json_floorheat, floorheat, string, canCpuID);
	s2j_json_set_basic_element(json_floorheat, floorheat, string, devName);
	s2j_json_set_basic_element(json_floorheat, floorheat, string, roomName);
	s2j_json_set_basic_element(json_floorheat, floorheat, int, devType);
	s2j_json_set_basic_element(json_floorheat, floorheat, int, devID);
	s2j_json_set_basic_element(json_floorheat, floorheat, int, bOnOff);
	s2j_json_set_basic_element(json_floorheat, floorheat, int, tempGet);
	s2j_json_set_basic_element(json_floorheat, floorheat, int, tempSet);
	s2j_json_set_basic_element(json_floorheat, floorheat, int, powChn);
	s2j_json_set_basic_element(json_floorheat, floorheat, int, autoRun);

	return json_floorheat;
}

static cJSON *create_ctl_reply_info(JsonContext *p, cJSON *root) {
	cJSON *dev_rows;

	JsonBase *base = (JsonBase *) p->data;
	root = create_base_json(base, root);

	cJSON_AddItemToObject(root, "dev_rows", dev_rows = cJSON_CreateArray());
	WARE_DEV *ware_dev = (WARE_DEV *) base->data;

	uint32_t canCpuID[25] = {0};
	char devName[25] = {0};
	char roomName[25] = {0};
	bytes_to_string(ware_dev->canCpuId, canCpuID, 12);
	//if (p->datType == e_udpPro_editDev) {
	bytes_to_string(ware_dev->devName, devName, strlen(ware_dev->devName));
	bytes_to_string(ware_dev->roomName, roomName, strlen(ware_dev->roomName));
	//}

	switch (ware_dev->devType) {
		case e_ware_airCond: {
			DEV_PRO_AIRCOND *air = (DEV_PRO_AIRCOND *) ware_dev->dat;

			aircond_struct item =
				{(char *) canCpuID, devName, roomName, ware_dev->devType, ware_dev->devId,
					air->bOnOff, air->selMode,
					air->selTemp, air->selSpd, air->selDirect, air->rev1, air->powChn};
			cJSON *air_item = struct_air_to_json(&item);

			cJSON_AddItemToArray(dev_rows, air_item);
		}
			break;
		case e_ware_light: {
			DEV_PRO_LIGHT *light = (DEV_PRO_LIGHT *) ware_dev->dat;
			light_struct item =
				{(char *) canCpuID, devName, roomName, ware_dev->devType, ware_dev->devId,
					light->bOnOff,
					light->bTuneEn, light->lmVal, light->powChn};

			cJSON *light_item = struct_light_to_json(&item);
			cJSON_AddItemToArray(dev_rows, light_item);
		}
			break;
		case e_ware_curtain: {
			DEV_PRO_CURTAIN *curtain = (DEV_PRO_CURTAIN *) ware_dev->dat;

			curtain_struct item =
				{(char *) canCpuID, devName, roomName, ware_dev->devType, ware_dev->devId,
					curtain->bOnOff,
					curtain->timRun, curtain->powChn};

			cJSON *curtain_item = struct_curtain_to_json(&item);
			cJSON_AddItemToArray(dev_rows, curtain_item);
		}
			break;
		case e_ware_fresh_air: {

			bytes_to_string(ware_dev->roomName, roomName, strlen(ware_dev->roomName));
			DEV_PRO_FRESHAIR *frair = (DEV_PRO_FRESHAIR *) ware_dev->dat;

			frair_struct item =
				{(char *) canCpuID, devName, roomName, ware_dev->devType, ware_dev->devId, frair->bOnOff,
					frair->spdSel, frair->onOffChn, frair->spdLowChn, frair->spdMidChn, frair->spdHighChn,
					frair->autoRun, frair->valPm10, frair->valPm25};

			cJSON *frair_item = struct_frair_to_json(&item);
			cJSON_AddItemToArray(dev_rows, frair_item);
		}
			break;
		case e_ware_floor_heat: {
			DEV_PRO_FLOOR_HEAT *floor_heat = (DEV_PRO_FLOOR_HEAT *) ware_dev->dat;

			floor_heat_struct item =
				{(char *) canCpuID, devName, roomName, ware_dev->devType, ware_dev->devId, floor_heat->bOnOff,
					floor_heat->tempGet, floor_heat->tempSet, floor_heat->powChn, floor_heat->autoRun};

			cJSON *frair_item = struct_floorheat_to_json(&item);
			cJSON_AddItemToArray(dev_rows, frair_item);
		}
			break;

		default: break;
	}

	return root;
}

char *create_ctl_reply_info_json(UDPPROPKT *pkt) {
	char uid[25] = {0};
	bytes_to_string(pkt->uidSrc, (uint32_t *) uid, 12);

	char pass[9] = {0};
	memcpy(pass, uid, 8);

	JsonBase base = {uid, pass, pkt->datType, pkt->subType1, pkt->subType2, pkt->datLen, pkt->dat};
	JsonContext jxt = {&base, create_ctl_reply_info};
	char *str = create_json(&jxt);
	return str;
}

static cJSON *create_dev_info(JsonContext *p, cJSON *root) {
	JsonBase *base = (JsonBase *) p->data;

	cJSON *dev_rows;

	root = create_base_json(base, root);

	cJSON_AddItemToObject(root, "dev_rows", dev_rows = cJSON_CreateArray());
	WARE_DEV *ware_dev = (WARE_DEV *) base->data;
	uint8_t canCpuID[25] = {0};
	bytes_to_string((uint8_t *) ware_dev->canCpuId, canCpuID, 12);

	int dev_cnt = 0;
	switch (base->subType2) {
		case e_ware_airCond: {
			dev_cnt = base->data_len / WARE_AIR_SIZE;

			for (int i = 0; i < dev_cnt; i++) {
				WARE_DEV *ware_dev = (WARE_DEV *) (base->data + i * WARE_AIR_SIZE);

				char devName[25] = {0};
				if (strlen(ware_dev->devName) > 12)
					bytes_to_string(ware_dev->devName, devName, 12);
				else
					bytes_to_string(ware_dev->devName, devName, strlen(ware_dev->devName));

				char roomName[25] = {0};
				if (strlen(ware_dev->roomName) > 12)
					bytes_to_string(ware_dev->roomName, roomName, 12);
				else
					bytes_to_string(ware_dev->roomName, roomName, strlen(ware_dev->roomName));

				DEV_PRO_AIRCOND *air = (DEV_PRO_AIRCOND *) ware_dev->dat;
				aircond_struct item =
					{(char *) canCpuID, devName, roomName, ware_dev->devType, ware_dev->devId,
						air->bOnOff, air->selMode,
						air->selTemp, air->selSpd, air->selDirect, air->rev1, air->powChn};
				cJSON *air_item = struct_air_to_json(&item);

				cJSON_AddItemToArray(dev_rows, air_item);
			}
			cJSON_AddItemToObject(root, "air", cJSON_CreateNumber(dev_cnt));
		}
			break;
		case e_ware_light: {
			dev_cnt = base->data_len / WARE_LGT_SIZE;

			for (int i = 0; i < dev_cnt; i++) {
				WARE_DEV *ware_dev = (WARE_DEV *) (base->data + i * WARE_LGT_SIZE);

				char devName[25] = {0};
				if (strlen(ware_dev->devName) > 12)
					bytes_to_string(ware_dev->devName, devName, 12);
				else
					bytes_to_string(ware_dev->devName, devName, strlen(ware_dev->devName));

				char roomName[25] = {0};
				if (strlen(ware_dev->roomName) > 12)
					bytes_to_string(ware_dev->roomName, roomName, 12);
				else
					bytes_to_string(ware_dev->roomName, roomName, strlen(ware_dev->roomName));
				DEV_PRO_LIGHT *light = (DEV_PRO_LIGHT *) ware_dev->dat;
				light_struct item =
					{(char *) canCpuID, devName, roomName, ware_dev->devType, ware_dev->devId,
						light->bOnOff,
						light->bTuneEn, light->lmVal, light->powChn};

				cJSON *light_item = struct_light_to_json(&item);
				cJSON_AddItemToArray(dev_rows, light_item);
			}
			cJSON_AddItemToObject(root, "light", cJSON_CreateNumber(dev_cnt));
		}
			break;
		case e_ware_curtain: {
			dev_cnt = base->data_len / WARE_CUR_SIZE;

			for (int i = 0; i < dev_cnt; i++) {
				WARE_DEV *ware_dev = (WARE_DEV *) (base->data + i * WARE_CUR_SIZE);

				char devName[25] = {0};
				if (strlen(ware_dev->devName) > 12)
					bytes_to_string(ware_dev->devName, devName, 12);
				else
					bytes_to_string(ware_dev->devName, devName, strlen(ware_dev->devName));

				char roomName[25] = {0};
				if (strlen(ware_dev->roomName) > 12)
					bytes_to_string(ware_dev->roomName, roomName, 12);
				else
					bytes_to_string(ware_dev->roomName, roomName, strlen(ware_dev->roomName));

				DEV_PRO_CURTAIN *curtain = (DEV_PRO_CURTAIN *) ware_dev->dat;

				curtain_struct item =
					{(char *) canCpuID, devName, roomName, ware_dev->devType, ware_dev->devId,
						curtain->bOnOff,
						curtain->timRun, curtain->powChn};

				cJSON *curtain_item = struct_curtain_to_json(&item);
				cJSON_AddItemToArray(dev_rows, curtain_item);
			}
			cJSON_AddItemToObject(root, "curtain", cJSON_CreateNumber(dev_cnt));
		}
			break;
		case e_ware_tvUP:
		case e_ware_tv: {
			dev_cnt = base->data_len / WARE_TV_SIZE;

			for (int i = 0; i < dev_cnt; i++) {
				WARE_DEV *ware_dev = (WARE_DEV *) (base->data + i * WARE_TV_SIZE);

				char devName[25] = {0};
				if (strlen(ware_dev->devName) > 12)
					bytes_to_string(ware_dev->devName, devName, 12);
				else
					bytes_to_string(ware_dev->devName, devName, strlen(ware_dev->devName));

				char roomName[25] = {0};
				if (strlen(ware_dev->roomName) > 12)
					bytes_to_string(ware_dev->roomName, roomName, 12);
				else
					bytes_to_string(ware_dev->roomName, roomName, strlen(ware_dev->roomName));

				waredev_struct item = {(char *) canCpuID, devName, roomName, ware_dev->devType,
					ware_dev->devId};

				cJSON *waredev_item = struct_waredev_to_json(&item);
				cJSON_AddItemToArray(dev_rows, waredev_item);
			}
			cJSON_AddItemToObject(root, "tvDev", cJSON_CreateNumber(dev_cnt));
		}
			break;
		case e_ware_lock: {
			dev_cnt = base->data_len / WARE_LOCK_SIZE;

			for (int i = 0; i < dev_cnt; i++) {
				WARE_DEV *ware_dev = (WARE_DEV *) (base->data + i * WARE_LOCK_SIZE);

				char devName[25] = {0};
				if (strlen(ware_dev->devName) > 12)
					bytes_to_string(ware_dev->devName, devName, 12);
				else
					bytes_to_string(ware_dev->devName, devName, strlen(ware_dev->devName));

				char roomName[25] = {0};
				if (strlen(ware_dev->roomName) > 12)
					bytes_to_string(ware_dev->roomName, roomName, 12);
				else
					bytes_to_string(ware_dev->roomName, roomName, strlen(ware_dev->roomName));

				DEV_PRO_LOCK *lock = (DEV_PRO_LOCK *) ware_dev->dat;

				curtain_struct item =
					{(char *) canCpuID, devName, roomName, ware_dev->devType, ware_dev->devId,
						lock->bOnOff,
						lock->timRun, lock->bLockOut, lock->powChnOpen, lock->pwd};

				cJSON *lock_item = struct_lock_to_json(&item);
				cJSON_AddItemToArray(dev_rows, lock_item);
			}
			cJSON_AddItemToObject(root, "lock", cJSON_CreateNumber(dev_cnt));
		}
			break;
		case e_ware_value: {
			dev_cnt = base->data_len / WARE_VALUE_SIZE;

			for (int i = 0; i < dev_cnt; i++) {
				WARE_DEV *ware_dev = (WARE_DEV *) (base->data + i * WARE_VALUE_SIZE);

				char devName[25] = {0};
				if (strlen(ware_dev->devName) > 12)
					bytes_to_string(ware_dev->devName, devName, 12);
				else
					bytes_to_string(ware_dev->devName, devName, strlen(ware_dev->devName));

				char roomName[25] = {0};
				if (strlen(ware_dev->roomName) > 12)
					bytes_to_string(ware_dev->roomName, roomName, 12);
				else
					bytes_to_string(ware_dev->roomName, roomName, strlen(ware_dev->roomName));

				DEV_PRO_VALVE *valve = (DEV_PRO_VALVE *) ware_dev->dat;

				valve_struct item =
					{(char *) canCpuID, devName, roomName, ware_dev->devType, ware_dev->devId,
						valve->bOnOff,
						valve->timRun, valve->powChnOpen, valve->powChnClose};

				cJSON *valve_item = struct_valve_to_json(&item);
				cJSON_AddItemToArray(dev_rows, valve_item);
			}
			cJSON_AddItemToObject(root, "valve", cJSON_CreateNumber(dev_cnt));
		}
			break;
		case e_ware_fresh_air: {
			dev_cnt = base->data_len / WARE_FRAIR_SIZE;

			for (int i = 0; i < dev_cnt; i++) {
				WARE_DEV *ware_dev = (WARE_DEV *) (base->data + i * WARE_FRAIR_SIZE);

				char devName[25] = {0};
				if (strlen(ware_dev->devName) > 12)
					bytes_to_string(ware_dev->devName, devName, 12);
				else
					bytes_to_string(ware_dev->devName, devName, strlen(ware_dev->devName));

				char roomName[25] = {0};
				if (strlen(ware_dev->roomName) > 12)
					bytes_to_string(ware_dev->roomName, roomName, 12);
				else
					bytes_to_string(ware_dev->roomName, roomName, strlen(ware_dev->roomName));

				DEV_PRO_FRESHAIR *frair = (DEV_PRO_FRESHAIR *) ware_dev->dat;

				frair_struct item =
					{(char *) canCpuID, devName, roomName, ware_dev->devType, ware_dev->devId, frair->bOnOff,
						frair->spdSel, frair->onOffChn, frair->spdLowChn, frair->spdMidChn, frair->spdHighChn,
						frair->autoRun, frair->valPm10, frair->valPm25};

				cJSON *frair_item = struct_frair_to_json(&item);
				cJSON_AddItemToArray(dev_rows, frair_item);
			}
			cJSON_AddItemToObject(root, "frair", cJSON_CreateNumber(dev_cnt));
		}
			break;
		case e_ware_floor_heat: {
			dev_cnt = base->data_len / WARE_FLOOR_HEAT_SIZE;

			for (int i = 0; i < dev_cnt; i++) {
				WARE_DEV *ware_dev = (WARE_DEV *) (base->data + i * WARE_FLOOR_HEAT_SIZE);

				char devName[25] = {0};
				if (strlen(ware_dev->devName) > 12)
					bytes_to_string(ware_dev->devName, devName, 12);
				else
					bytes_to_string(ware_dev->devName, devName, strlen(ware_dev->devName));

				char roomName[25] = {0};
				if (strlen(ware_dev->roomName) > 12)
					bytes_to_string(ware_dev->roomName, roomName, 12);
				else
					bytes_to_string(ware_dev->roomName, roomName, strlen(ware_dev->roomName));
				DEV_PRO_FLOOR_HEAT *floor_heat = (DEV_PRO_FLOOR_HEAT *) ware_dev->dat;

				floor_heat_struct item =
					{(char *) canCpuID, devName, roomName, ware_dev->devType, ware_dev->devId, floor_heat->bOnOff,
						floor_heat->tempGet, floor_heat->tempSet, floor_heat->powChn, floor_heat->autoRun};

				cJSON *frair_item = struct_floorheat_to_json(&item);
				cJSON_AddItemToArray(dev_rows, frair_item);
			}
			cJSON_AddItemToObject(root, "floorheat", cJSON_CreateNumber(dev_cnt));
		}
			break;
		default: break;
	}
	return root;
}

char *create_dev_info_json(UDPPROPKT *pkt) {
	char uid[25] = {0};
	bytes_to_string(pkt->uidSrc, (uint32_t *) uid, 12);

	char pass[9] = {0};
	memcpy(pass, uid, 8);

	JsonBase base = {uid, pass, pkt->datType, pkt->subType1, pkt->subType2, pkt->datLen, pkt->dat};
	JsonContext jxt = {&base, create_dev_info};
	char *str = create_json(&jxt);
	return str;
}

typedef struct {
  char *canCpuID;
  int devID;
  int devType;
  int lmVal;
  int rev2;
  int rev3;
  int bOnOff;
  int param1;
  int param2;
} run_dev_item_struct;

typedef struct {
  char *secName;//防区名称
  char *secCode;//红外探测器无线信号编码
  int secId;//防区 Id，唯一标识这个防区
  int secType;//布防类型 0--24 小时布防 1--在家布防 2--外出布防 0xff--撤防状态
  int sceneId;//防区触发情景模式事件 id 不触发则为 0xff
  int secDev;//防区是否触发设备动作 0--不触发 1--触发
  int devCnt;//itemAry 的有效个数
  int rev1;//
  int rev2;//
  int valid;//防区是否有效
} security_struct;

static cJSON *struct_security_to_json(void *struct_obj) {
	security_struct *security = (security_struct *) struct_obj;

	s2j_create_json_obj(json_security);

	s2j_json_set_basic_element(json_security, security, string, secName);
	s2j_json_set_basic_element(json_security, security, string, secCode);
	s2j_json_set_basic_element(json_security, security, int, secId);
	s2j_json_set_basic_element(json_security, security, int, secType);
	s2j_json_set_basic_element(json_security, security, int, sceneId);
	s2j_json_set_basic_element(json_security, security, int, secDev);
	s2j_json_set_basic_element(json_security, security, int, devCnt);
	s2j_json_set_basic_element(json_security, security, int, rev1);
	s2j_json_set_basic_element(json_security, security, int, rev2);
	s2j_json_set_basic_element(json_security, security, int, valid);

	return json_security;
}

static cJSON *create_set_security_reply(JsonContext *p, cJSON *root) {
	JsonBase *base = (JsonBase *) p->data;
	root = create_base_json(base, root);
	if (base->subType1 == 2) {
		SEC_ALRM *sec_alrm = (SEC_ALRM *) base->data;
		cJSON_AddItemToObject(root, "secStatus", cJSON_CreateNumber(sec_alrm->secStatus));
		cJSON_AddItemToObject(root, "secDat", cJSON_CreateNumber(sec_alrm->secDat));
	} else if (base->subType1 == 4 || base->subType1 == 6 || base->subType1 == 7) {
		cJSON *sec_info_rows, *item, *itemAry;

		cJSON_AddItemToObject(root, "sec_info_rows", sec_info_rows = cJSON_CreateArray());

		int cnt = base->data_len / sizeof(SEC_INFO);
		for (int i = 0; i < cnt; i++) {
			SEC_INFO *sec_info = (SEC_INFO *) (base->data + i * sizeof(SEC_INFO));

			uint32_t secName[ID_NAME_LEN_STR] = {0};
			if (strlen(sec_info->secName) > 12)
				bytes_to_string(sec_info->secName, secName, 12);
			else
				bytes_to_string(sec_info->secName, secName, strlen(sec_info->secName));
			uint32_t secCode[ID_NAME_LEN_STR] = {0};
			bytes_to_string(sec_info->secCode, secCode, 12);

			security_struct sec_uct =
				{secName, secCode, sec_info->secId, sec_info->secType, sec_info->sceneId,
					sec_info->secDev,
					sec_info->itemCnt, sec_info->rev1, sec_info->rev2, sec_info->valid};
			item = struct_security_to_json(&sec_uct);

			cJSON_AddItemToObject(item, "run_dev_item", itemAry = cJSON_CreateArray());
			for (int j = 0; j < sec_info->itemCnt; j++) {
				RUN_DEV_ITEM run_dev_item = (RUN_DEV_ITEM) sec_info->itemAry[j];

				uint32_t canCpuID[ID_NAME_LEN_STR] = {0};
				bytes_to_string(run_dev_item.uid, canCpuID, 12);
				run_dev_item_struct
					item = {(char *) canCpuID, run_dev_item.devID, run_dev_item.devType,
					run_dev_item.lmVal,
					run_dev_item.rev2, run_dev_item.rev3, run_dev_item.bOnoff,
					run_dev_item.param1,
					run_dev_item.param2};

				cJSON *item_json = struct_run_dev_item_to_json(&item);
				cJSON_AddItemToArray(itemAry, item_json);
			}
			cJSON_AddItemToArray(sec_info_rows, item);
		}
		cJSON_AddItemToObject(root, "itemCnt", cJSON_CreateNumber(cnt));
	}

	return root;
}

char *create_set_security_reply_json(UDPPROPKT *pkt) {
	char uid[25] = {0};
	bytes_to_string(pkt->uidSrc, (uint8_t *) uid, 12);

	char pass[9] = {0};
	memcpy(pass, uid, 8);

	JsonBase base = {uid, pass, pkt->datType, pkt->subType1, pkt->subType2, pkt->datLen, pkt->dat};
	JsonContext jxt = {&base, create_set_security_reply};
	char *str = create_json(&jxt);
	return str;
}

