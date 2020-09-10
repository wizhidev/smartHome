#ifndef JSON_H
#define JSON_H

#include "../comm/udppropkt.h"

extern char *create_events_json(UDPPROPKT *pkt);

extern char* create_board_chnout_json(UDPPROPKT *pkt);

extern char* create_board_keyinput_json(UDPPROPKT *pkt);

extern char* create_rcu_json(UDPPROPKT *pkt);

extern char *create_dev_info_json(UDPPROPKT *pkt);

extern char* create_ctl_reply_info_json(UDPPROPKT *pkt);

extern char *create_chn_status_json(UDPPROPKT *pkt);

extern char *create_set_security_reply_json(UDPPROPKT *pkt);
#endif // JSON_H
