//
// Created by hwp on 2017/11/22.
//

#ifndef SHOME_LIB_UDP_H
#define SHOME_LIB_UDP_H

#include <netinet/in.h>
#include "../comm/comm.h"
#include "../comm/str_comm.h"
typedef struct _SRequest{
  socklen_t fromlen;
  struct sockaddr_in from;
  char buf[BUFF_SIZE];
}SRequest;

void register_socket_id(int socket_id);
void register_ui_socket_id(int socket_id);
//广播包
void udp_broadcast(uint8_t *data, int size);
//创建udp客户端
int init_udp_client_create();
void *ui_udp_server();
//创建unix socket服务端
int unix_socket_udp_server();
//unix socket发送
ssize_t send_unix_udp_server(char *buffer, size_t size);
//创建inet socket服务端
int create_udp_server_socket(int port);
//inet socket数据处理
void recv_inet_udp_server(int socket_id, SRequest *sRequest, DataExtract dataExtract);
//inet 发送数据
ssize_t send_inet_udp_server(struct sockaddr_in server, u_int8_t *buffer, int size);

#endif //SHOME_LIB_UDP_H
