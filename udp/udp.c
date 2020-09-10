//
// Created by hwp on 2017/11/22.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <unistd.h>
#include <sys/un.h>
#include <errno.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include "udp.h"
#include "../comm/log.h"
#include "network.h"
#include "../incharge/extract_data.h"

#define UNIX_DOMAIN_PATH "/tmp/unix_domain_path_client.sock"
#define UNIX_DOMAIN_SERVER_PATH "/tmp/unix_domain_path_server.sock"
int udp_server_id;
int ui_socket_id;
int unix_socket_clint_id;

static int udp_unix_socket_create(char *unix_path) {
	int sock_fd = -1;
	struct sockaddr_un unix_addr;

	if (NULL == unix_path) {
		log_error("");
		return -1;
	}

	sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (-1 == sock_fd) {
		log_error("create AF_UNIX failed.");
		return -1;
	}

	unlink(unix_path);

	bzero(&unix_addr, sizeof(struct sockaddr_un));
	unix_addr.sun_family = AF_UNIX;
	strncpy(unix_addr.sun_path, unix_path, sizeof(unix_addr.sun_path) - 1);

	if (bind(sock_fd, (const struct sockaddr *) &unix_addr,
			 sizeof(struct sockaddr_un)) < 0) {
		log_error("bind failed, %s", strerror(errno));
		close(sock_fd);
		return RET_FAIL;
	}

	return sock_fd;
}

static ssize_t udp_unix_socket_send(int fd, char *buffer, size_t size) {
	struct sockaddr_un peer_addr;
	ssize_t bytes_sent;

	bzero(&peer_addr, sizeof(peer_addr));
	peer_addr.sun_family = AF_UNIX;
	strncpy(peer_addr.sun_path, UNIX_DOMAIN_PATH, strlen(UNIX_DOMAIN_PATH));

	bytes_sent = sendto(fd, buffer, size, 0,
						(struct sockaddr *) &peer_addr,
						sizeof(struct sockaddr_un));
	if (bytes_sent == -1) {
		log_error("unix domain socket sendto failed, %s", strerror(errno));
	}
	if (bytes_sent != size) {
		log_error("sendto: bytes sent: %d, need sent: %d", bytes_sent, size);
	}

	return bytes_sent;
}

static ssize_t udp_unix_socket_recv(int fd, char *buffer, size_t size) {
	struct sockaddr_un peer_addr;
	socklen_t addr_len;
	ssize_t bytes_recv;

	addr_len = sizeof(struct sockaddr_un);

	bytes_recv = recvfrom(fd, buffer, size, 0, (struct sockaddr *) &peer_addr, &addr_len);

	if (bytes_recv == -1) {
		if (EAGAIN == errno) {
			return 0;
		}
		log_error("recvfrom error, %s", strerror(errno));
		return -1;
	}

	return bytes_recv;
}

static void udp_unix_socket_close(int fd) {
	close(fd);
}

static int unix_socket_udp_client() {

	unix_socket_clint_id = udp_unix_socket_create(UNIX_DOMAIN_SERVER_PATH);
	if (unix_socket_clint_id == RET_FAIL) {
		log_error("udp_unix_socket_create failed.");
		return RET_FAIL;
	}

	return RET_OK;
}

int unix_socket_udp_server() {
	int sock_fd = RET_FAIL;
	char buffer[1024 * 5];
	ssize_t bytes_recv = RET_FAIL;

	log_set_level(LOG_LEVEL_DEBUG);
	unix_socket_udp_client();

	sock_fd = udp_unix_socket_create(UNIX_DOMAIN_SERVER_PATH);
	if (sock_fd < 0) {
		log_error("udp_unix_socket_create failed.");
		return RET_FAIL;
	}

	sock_set_nonblocking(sock_fd);

	while (1) {
		bzero(buffer, sizeof(buffer));
		bytes_recv = udp_unix_socket_recv(sock_fd, buffer, sizeof(buffer) - 1);
		if (bytes_recv > 0) {
			log_debug("%d %s", bytes_recv, buffer);
			parser_data_fromui(buffer);
		}
	}

	udp_unix_socket_close(sock_fd);

	return RET_OK;
}

static int init_inet_socket_udp_server(int port) {
	struct sockaddr_in server_addr;
	int udp_sock_fd;

	udp_sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (RET_FAIL == udp_sock_fd) {
		log_error("create udp socket failed, %s.", strerror(errno));
		return RET_FAIL;
	}

	if (!sock_set_nonblocking(udp_sock_fd)) {
		close(udp_sock_fd);
		return -1;
	}
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);;
	server_addr.sin_port = htons(port);

	if (bind(udp_sock_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
		log_error("bind udp socket failed, %s.", strerror(errno));
		close(udp_sock_fd);
		return RET_FAIL;
	}

	return udp_sock_fd;
}

static ssize_t udp_inet_socket_recv(int fd, SRequest *sRequest) {
	socklen_t addr_len;
	ssize_t bytes_recv;

	addr_len = sizeof(sRequest->from);

	bytes_recv = recvfrom(fd, sRequest->buf, sizeof(sRequest->buf) - 1, 0,
						  (struct sockaddr *) &sRequest->from, &addr_len);

	if (bytes_recv == -1) {
		if (EAGAIN == errno) {
			return 0;
		}
		log_error("recvfrom error, %s", strerror(errno));
		return -1;
	}

	return bytes_recv;
}

static ssize_t udp_inet_socket_send(int fd, struct sockaddr_in server, uint8_t *buffer, int size) {
	ssize_t bytes_sent;

	bytes_sent = sendto(fd, buffer, size, 0,
						(struct sockaddr *) &server,
						sizeof(server));
	if (bytes_sent == -1) {
		log_error("udp_inet sendto failed, %s", strerror(errno));
	}
	if (bytes_sent != size) {
		log_error("udp_inet sendto: bytes sent: %d, need sent: %d", bytes_sent, size);
	}
	return bytes_sent;
}
static struct sockaddr_in create_broadcast_addr(int fd) {
	int optval = 1;//这个值一定要设置，否则可能导致sendto()失败
	setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(int));
	struct sockaddr_in Addr;
	memset(&Addr, 0, sizeof(struct sockaddr_in));
	Addr.sin_family = AF_INET;
	Addr.sin_addr.s_addr = inet_addr(BROADCAST_IP);
	Addr.sin_port = htons(GW_SERVER_PORT);

	return Addr;
}

void register_socket_id(int socket_id) {
	udp_server_id = socket_id;
}

void register_ui_socket_id(int socket_id) {
	ui_socket_id = socket_id;
}

void udp_broadcast(uint8_t *data, int size) {
	struct sockaddr_in addr = create_broadcast_addr(udp_server_id);
	send_inet_udp_server(addr, data, size);
}

int create_udp_server_socket(int port) {
	return init_inet_socket_udp_server(port);
}

ssize_t send_unix_udp_server(char *buffer, size_t size) {
	struct sockaddr_in Addr;
	memset(&Addr, 0, sizeof(struct sockaddr_in));
	Addr.sin_family = AF_INET;
	Addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	Addr.sin_port = htons(25001);

	udp_inet_socket_send(ui_socket_id, Addr, buffer, size);
	//return udp_unix_socket_send(unix_socket_clint_id, buffer, size);
}

void *ui_udp_server() {
	while (1) {
		SRequest *request = (SRequest *) malloc(sizeof(SRequest));
		ssize_t bytes_recv = udp_inet_socket_recv(ui_socket_id, request);
		if (bytes_recv > 0) {
			log_debug("%d %s", bytes_recv, request->buf);
			parser_data_fromui(request->buf);
		}
		free(request);
	}
}

ssize_t send_inet_udp_server(struct sockaddr_in server, u_int8_t *buffer, int size) {
	return udp_inet_socket_send(udp_server_id, server, buffer, size);
}

void recv_inet_udp_server(int socket_id, SRequest *sRequest, DataExtract dataExtract) {

	ssize_t bytes_recv = udp_inet_socket_recv(socket_id, sRequest);
	if (bytes_recv > 0) {
		dataExtract(sRequest);
	} else {
		return;
	}
}