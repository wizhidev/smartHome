//
// Created by hwp on 2017/11/21.
//

#ifndef SHOME_LIB_TO_GWCLIENT_H
#define SHOME_LIB_TO_GWCLIENT_H

#include "../udp/udp.h"
extern char LOCAL_IP[16];
Ret get_local_ip();
void create_udp_broadcast(uint8_t *devUnitID);
UDPPROPKT * create_ask_pkt(UDPPROPKT *udp_pro_pkt);
UDPPROPKT * create_handshake_pkt(UDPPROPKT *pkt);
UDPPROPKT *pre_send_udp_pkt(u_int8_t *dat, int dat_len, u_int8_t cmd, u_int8_t *rcu_id,
							int ack, int sub_type1, int sub_type2);
void send_data_to_gw(u_int8_t *devUnitID, void *str, int datlen);
#endif //SHOME_LIB_TO_GWCLIENT_H
