#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include "data_list/gw_list.h"
#include "global.h"
#include "pthread/locker_pthread.h"
#include "udp/udp.h"
#include "pthread/pthread_pool.h"
#include "udp/network.h"
#include "incharge/extract_data.h"
#include "incharge/to_gwclient.h"
#include "time_task/timer.h"

SList *gw_list;
SList *sn_list;
static threadpool thpool; //线程池

static Ret check_invalid_ip(SRequest *request) {
	if (is_valid_ip(inet_ntoa(request->from.sin_addr))) {
		if (memcmp(inet_ntoa(request->from.sin_addr), LOCAL_IP, 16) == 0)
			return RET_FAIL;
		else
			return RET_OK;
	} else {
		return RET_FAIL;
	}
}

void extract_data(void *data){
	SRequest *request = (SRequest *)data;
	if (check_invalid_ip(request) == RET_OK) {
		thpool_add_work(thpool, (void *) parser_data_fromgw, (void *) request);
		//parser_data_fromgw(request);
	} else {
		return;
	}
}

void udp_server() {
	if (get_local_ip() != RET_OK)
		return;
	thpool = thpool_init(20);

	gw_list = slist_create(NULL, NULL, locker_pthread_create());
	sn_list = slist_create(NULL, NULL, locker_pthread_create());


	int socket_id = create_udp_server_socket(GW_SERVER_PORT);
	int ui_socket_id = create_udp_server_socket(UI_SERVER_PORT);
	register_socket_id(socket_id);
	register_ui_socket_id(ui_socket_id);

	thpool_add_work(thpool, (void *) ui_udp_server, NULL);
	thpool_add_work(thpool, (void *)singal_msg, NULL);

	setTimer(1 * 60, TIMER_SEARCH_DEV_SPEC);
	setTimer(2 * 60, TIMER_SEARCH_DEL_SN);
	setTimer(30 * 60, TIMER_SEARCH_DEL_GW);


	while (1) {
		SRequest *request = (SRequest *) malloc(sizeof(SRequest));
		recv_inet_udp_server(socket_id, request, extract_data);
		free(request);
	}

}

int main() {
	udp_server();
	return 0;
}