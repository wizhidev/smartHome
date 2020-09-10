//
// Created by hwp on 2017/11/23.
//

#ifndef SHOME_LIB_STR_COMM_H
#define SHOME_LIB_STR_COMM_H
#include <memory.h>

#define GW_SERVER_PORT        8300
#define UI_SERVER_PORT        8400
#define UI_CLIENT_PORT        25001
#define BUFF_SIZE             1024*5
#define HEAD_STRING           "head"

#define ID_LEN_BYTE  12
#define ID_NAME_LEN_STR 25
#define PASS_LEN_STR  8
#define NAME_LEN_BYTE 12
#define IP_LEN_BYTE     4
#define MAC_LEN_BYTE 6

#define BROADCAST_IP "255.255.255.255"
int mm_string_strstr(char *big, char *key, int where_start);
int mm_string_cut_by_where_start_and_end(const uint8_t *s_in, int start, int end, uint8_t *s_out, int with_0);
void substring(char *dest,char *src,int start,int end);
int itoa_bin(uint16_t data, char *str);
int bytes_to_string(const uint8_t * pSrc, uint8_t * pDst, int nSrcLength);
int string_to_bytes(const char* pSrc, uint8_t * pDst, int nSrcLength);
#endif //SHOME_LIB_STR_COMM_H
