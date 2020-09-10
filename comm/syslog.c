//
// Created by hwp on 2017/11/8.
//

#include <memory.h>
#include <printf.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include "syslog.h"

static char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                         "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

static char syslog_ip[32]       = {0};
static int  syslog_port         = 0;
static int  syslog_sock_fd      = -1;
static int  syslog_facility     = -1;
static char syslog_hostname[32] = {0};
static char syslog_program[32]  = {0};
static struct sockaddr_in syslog_addr;

int syslog_open(char *ip, int port, int facility, char *hostname, char *program)
{
    if(NULL == ip){
        printf("ip is null.\n");
        return 0;
    }

    if(NULL == program){
        printf("program is null.\n");
        return 0;
    }

    bzero(syslog_ip, sizeof(syslog_ip));
    strncpy(syslog_ip, ip, sizeof(syslog_ip) - 1);
    syslog_port = port;

    bzero(&syslog_addr, sizeof(struct sockaddr_in));
    syslog_addr.sin_family = AF_INET;
    syslog_addr.sin_port   = htons(syslog_port);
    if(inet_aton(syslog_ip, &syslog_addr.sin_addr) < 0){
        printf("inet_aton for %s failed, %s\n", syslog_ip, strerror(errno));
        return -1;
    }

    syslog_sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(syslog_sock_fd < 0){
        printf("create syslog udp socket failed, %s\n", strerror(errno));
        return -1;
    }

    if(facility < SYSLOG_KERN || facility> SYSLOG_LOCAL7){
        printf("facility <SYSLOG_KERN or facility> SYSLOG_LOCAL7.\n");
        return -1;
    }
    syslog_facility = facility;

    bzero(syslog_hostname, sizeof(syslog_hostname));
    if(hostname){
        strncpy(syslog_hostname, hostname, sizeof(syslog_hostname) - 1);
    }

    bzero(syslog_program, sizeof(syslog_program));
    strncpy(syslog_program, program, sizeof(syslog_program) - 1);

    return 1;
}

/* syslog 日志按照 BSD-syslog or legacy-syslog messages 格式构造 */
/* 例子：<priority>timestamp hostname program[pid]: message */
int syslog_write(int level, char *msg, int length)
{
    char buffer[1024] = {0};
    int  offset = 0;
    int  result = 0;

    struct timeval cur_time;
    struct tm      tm;

    if(level < SYSLOG_EMERG || level> SYSLOG_DEBUG){
        printf("level <SYSLOG_EMERG or level> SYSLOG_DEBUG.\n");
        return 0;
    }

    if(syslog_sock_fd < 0){
        printf("syslog_sock_fd < 0.\n");
        return 0;
    }

    /* 构造 PRI 域 */
    result = snprintf(buffer, sizeof(buffer), "<%d>", SYSLOG_MAKEPRI(syslog_facility, level));
    offset += result;

    /* 构造 HEADER 域 */
    /* HEADER 域包含时间戳和主机名 */
    gettimeofday(&cur_time, NULL);
    localtime_r(&cur_time.tv_sec, &tm);
    result = snprintf(buffer + offset, sizeof(buffer) - offset,
                      "%s %2d %02d:%02d:%02d %s ",
                      months[tm.tm_mon - 1],
                      tm.tm_mday,
                      tm.tm_hour,
                      tm.tm_min,
                      tm.tm_sec,
                      syslog_hostname);
    offset += result;

    /* 构造 MSG 域 */
    /* MSG 域: program[pid]: msg text */
    result = snprintf(buffer + offset, sizeof(buffer) - offset,
                      "%s[%d]: ",
                      syslog_program,
                      getpid());
    offset += result;

    if(sizeof(buffer) - offset > (unsigned)length){
        memcpy(buffer + offset, msg, length);
        offset += length;
    }
    else{
        printf("msg is too long , truncate it.\n");
        memcpy(buffer + offset, msg, sizeof(buffer) - offset);
        offset = sizeof(buffer);
    }

    if(sendto(syslog_sock_fd, buffer, offset, 0,
              (struct sockaddr *)&syslog_addr,
              sizeof(struct sockaddr_in)) < 0){
        printf("syslog sendto failed, %s.\n", strerror(errno));
        return 0;
    }
    return 1;
}

int syslog_close()
{
    if(syslog_sock_fd>= 0){
        close(syslog_sock_fd);
        syslog_sock_fd = -1;
    }
    return 1;
}

int syslog_test(void)
{
    syslog_write(SYSLOG_INFO, "test syslog-ng msg", strlen("test syslog-ng msg"));
    syslog_close();
    syslog_open("127.0.0.1", 8888, SYSLOG_LOCAL0, "www.iqiyi.com", "test");
    syslog_write(SYSLOG_DEBUG, "test syslog-ng msg", strlen("test syslog-ng msg"));
    syslog_write(SYSLOG_DEBUG, "test syslog-ng msg", strlen("test syslog-ng msg"));
    syslog_write(SYSLOG_DEBUG, "test syslog-ng msg", strlen("test syslog-ng msg"));
    syslog_write(SYSLOG_DEBUG, "test syslog-ng msg", strlen("test syslog-ng msg"));
    syslog_write(SYSLOG_DEBUG, "test syslog-ng msg", strlen("test syslog-ng msg"));

    syslog_write(SYSLOG_INFO, "test syslog-ng msg", strlen("test syslog-ng msg"));
    syslog_write(SYSLOG_INFO, "test syslog-ng msg", strlen("test syslog-ng msg"));
    syslog_write(SYSLOG_INFO, "test syslog-ng msg", strlen("test syslog-ng msg"));
    syslog_write(SYSLOG_INFO, "test syslog-ng msg", strlen("test syslog-ng msg"));
    syslog_write(SYSLOG_INFO, "test syslog-ng msg", strlen("test syslog-ng msg"));

    syslog_close();

    syslog_write(SYSLOG_INFO, "test syslog-ng msg", strlen("test syslog-ng msg"));
    syslog_close();
    return 1;
}