//
// Created by hwp on 2017/11/10.
//

#include <sys/socket.h>
#include <arpa/inet.h>
#include <memory.h>
#include <ifaddrs.h>
#include <fcntl.h>
#include <errno.h>
#include "../comm/log.h"

int sock_set_nonblocking(int sock_fd)
{
    int opts;
    opts = fcntl(sock_fd, F_GETFL);
    if(opts < 0){
        log_error("fcntl error, %s.", strerror(errno));
        return 0;
    }

    opts = opts | O_NONBLOCK;
    if(fcntl(sock_fd, F_SETFL, opts)<0){
        log_error("fcntl error, %s.", strerror(errno));
        return 0;
    }

    return 1;
}

int sock_set_rcv_buffer_size(int sock_fd, int size)
{
    int rcv_size = 0;
    socklen_t opt_len  = 0;

    opt_len = sizeof(rcv_size);
    if(getsockopt(sock_fd, SOL_SOCKET, SO_RCVBUF, &rcv_size, &opt_len) < 0){
        log_error("getsockopt failed, %s.", strerror(errno));
        return 0;
    }

    rcv_size = size;
    opt_len  = sizeof(rcv_size);
    if(setsockopt(sock_fd, SOL_SOCKET, SO_RCVBUF, &rcv_size, opt_len) < 0){
        log_error("setsockopt failed, %s.", strerror(errno));
        return 0;
    }
    return 1;
}

int sock_set_snd_buffer_size(int sock_fd, int size)
{
    int snd_size = 0;
    socklen_t opt_len  = 0;

    opt_len = sizeof(snd_size);
    if(getsockopt(sock_fd, SOL_SOCKET, SO_SNDBUF, &snd_size, &opt_len) < 0){
        log_error("getsockopt failed, %s.", strerror(errno));
        return 0;
    }

    snd_size = size;
    opt_len  = sizeof(snd_size);
    if(setsockopt(sock_fd, SOL_SOCKET, SO_SNDBUF, &snd_size, opt_len) < 0){
        log_error("setsockopt failed, %s.", strerror(errno));
        return 0;
    }
    return 1;
}

int is_valid_ip(const char *ip_str)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ip_str, &(sa.sin_addr));

    if (result == 0)
    {
        return result;
    }

    return 1;
}

int set_reuse_addr(int sock_fd){
    int opt_val       = 1;
    socklen_t opt_len = sizeof(int);
    int ret;

    ret = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, opt_len);
    return ret < 0 ? 0 : 1;
}

int get_ip(const char* version, char* address)
{
    struct ifaddrs * ifAddrStruct=NULL;
    void * tmpAddrPtr=NULL;

    getifaddrs(&ifAddrStruct);

    while (ifAddrStruct!=NULL) {
        if (ifAddrStruct->ifa_addr->sa_family==AF_INET) {
            if ((0 == strcmp(ifAddrStruct->ifa_name,"en0") ||
                 0 == strcmp(ifAddrStruct->ifa_name,"eth0")) &&
                0 == strcmp(version,"v4")) {
                tmpAddrPtr = &((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
                inet_ntop(AF_INET, tmpAddrPtr, address, INET_ADDRSTRLEN);
                return 1;
            }
        }
        else if (ifAddrStruct->ifa_addr->sa_family==AF_INET6) {
            if ((0 == strcmp(ifAddrStruct->ifa_name,"en0") ||
                 0 == strcmp(ifAddrStruct->ifa_name,"eth0")) &&
                0 == strcmp(version,"v6")) {
                tmpAddrPtr=&((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
                inet_ntop(AF_INET6, tmpAddrPtr, address, INET6_ADDRSTRLEN);
                return 1;
            }
        }
        ifAddrStruct = ifAddrStruct->ifa_next;
    }
    return 0;
}