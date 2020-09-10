//
// Created by hwp on 2017/11/22.
//

#include <memory.h>
#include <stdlib.h>
#include "to_uiclient.h"
#include "format_bin_to_json.h"
#include "../udp/udp.h"

static void report_rcu_info_json(UDPPROPKT *pkt) {
	char *json_str = create_rcu_json(pkt);
	send_unix_udp_server(json_str, strlen(json_str));
	free(json_str);
}

static void report_all_devs_info_json(UDPPROPKT *pkt) {
	char *json_str = create_dev_info_json(pkt);
	send_unix_udp_server(json_str, strlen(json_str));
	free(json_str);
}
static void report_all_ctl_reply_json(UDPPROPKT *pkt) {
	char *json_str = create_ctl_reply_info_json(pkt);
	send_unix_udp_server(json_str, strlen(json_str));
	free(json_str);
}
static void report_chn_status_json(UDPPROPKT *pkt) {
	char *json_str = create_chn_status_json(pkt);
	send_unix_udp_server(json_str, strlen(json_str));
	free(json_str);
}

static void report_scene_ctl_reply_json(UDPPROPKT *pkt) {
	char *json_str = create_events_json(pkt);
	if (json_str == NULL)
		return;
	send_unix_udp_server(json_str, strlen(json_str));
	free(json_str);
}

static void report_board_chnout_json(UDPPROPKT *pkt) {
	char *json_str = create_board_chnout_json(pkt);
	send_unix_udp_server(json_str, strlen(json_str));
	free(json_str);
}

static void report_board_keyinput_json(UDPPROPKT *pkt) {
	char *json_str = create_board_keyinput_json(pkt);
	send_unix_udp_server(json_str, strlen(json_str));
	free(json_str);
}

static void set_security_reply_json(UDPPROPKT *pkt) {
	char *json_str = create_set_security_reply_json(pkt);
	send_unix_udp_server(json_str, strlen(json_str));
	free(json_str);
}

void send_json_info_toui(void *data) {
	UDPPROPKT *pkt = (UDPPROPKT *) data;
	switch (pkt->datType) {
		case e_udpPro_getRcuInfo: {
			report_rcu_info_json(pkt);
		}
			break;
		case e_udpPro_getDevsInfo: {
			report_all_devs_info_json(pkt);
		}
			break;
//		case e_udpPro_addDev:
		case e_udpPro_ctrlDev:
//		case e_udpPro_editDev:
//		case e_udpPro_delDev:
			report_all_ctl_reply_json(pkt);
			break;
		case e_udpPro_getSceneEvents:
			report_scene_ctl_reply_json(pkt);
			break;
//
		case e_udpPro_addSceneEvents:
		case e_udpPro_editSceneEvents:
		case e_udpPro_exeSceneEvents:
		case e_udpPro_delSceneEvents: report_scene_ctl_reply_json(pkt);
			break;
		case e_udpPro_getBoards: {
			BOARD_CHNOUT *board = (BOARD_CHNOUT *) pkt->dat;
			switch (board->boardType) {
				case e_board_chnOut: {
					report_board_chnout_json(pkt);
				}
					break;
				case e_board_keyInput: {
					report_board_keyinput_json(pkt);
				}
				default: break;
			}
			break;
		}
//		case e_udpPro_getKeyOpItems: report_key_opitem_json(pkt);
//			break;
//		case e_udpPro_setKeyOpItems: set_key_opitem_reply_json(pkt);
//			break;
//		case e_udpPro_getChnOpItems: report_chn_opitem_json(pkt);
//			break;
//		case e_udpPro_setChnOpItems: set_chn_opitem_reply_json(pkt);
//			break;
		case e_udpPro_security_info: set_security_reply_json(pkt);
			break;
//		case e_udpPro_get_key2scene: get_key2scene_reply_json(pkt);
//			break;
//		case e_udpPro_set_key2scene: set_key2scene_reply_json(pkt);
//			break;
		case e_udpPro_chns_status: report_chn_status_json(pkt);
			break;
//		case e_udpPro_getTimerEvents://测试正常
//		case e_udpPro_addTimerEvents: //没有该功能
//		case e_udpPro_editTimerEvents://测试正常
//			get_timerEvents_reply_json(pkt);
//			break;
//		case e_udpPro_getEnvEvents://测试正常
//		case e_udpPro_editEnvEvents://测试正常
//			get_envEvents_reply_json(pkt);
//			break;
//		case e_udpPro_secs_trigger: set_secs_trigger_reply_json(pkt);
		default: break;
	}
}
