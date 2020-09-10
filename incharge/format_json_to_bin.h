//
// Created by hwp on 2017/11/23.
//

#ifndef SHOME_LIB_FORMAT_JSON_TO_BIN_H
#define SHOME_LIB_FORMAT_JSON_TO_BIN_H


void get_devs_pkt(uint8_t *devUnitID);
void get_scenes_pkt(uint8_t *devUnitID);
void get_board_pkt(uint8_t *devUnitID);
void get_security_pkt(uint8_t *devUnitID, int subType1, int subType2);
void ctrl_devs_pkt(uint8_t *devUnitID,
				   uint8_t *canCpuID,
				   int datType,
				   int devType,
				   int devID,
				   int powChn,
				   uint8_t *devName,
				   uint8_t *roomName,
				   int cmd,
				   char *buffer);
void ctrl_scene_pkt(uint8_t *devUnitID, int eventId, int cmd);
void set_security_pkt(uint8_t *devUnitID, int subType1, int subType2, char *buffer);
#endif //SHOME_LIB_FORMAT_JSON_TO_BIN_H
