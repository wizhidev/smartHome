//
// Created by hwp on 2017/11/10.
//

#ifndef CLIBRARIES_NETWORK_H
#define CLIBRARIES_NETWORK_H
/**
 *        Name: sock_set_nonblocking
 * Description: 。
 *   Parameter: sock_fd -> socket 描述符
 *      Return: 0 -> 设置失败。
 *              1 -> 设置成功。
 */
int sock_set_nonblocking(int sock_fd);

/**
 *        Name: sock_set_rcv_buffer_size
 * Description: 设置接收缓冲区大小
 *   Parameter: sockd_fd -> socket 描述符
 *              size -> 接收缓冲区大小
 *      Return: 0 -> 设置失败。
 *              1 -> 设置成功 。
 */
int sock_set_rcv_buffer_size(int sock_fd, int size);

/**
 *        Name: sock_set_snd_buffer_size
 * Description: 设置发送缓冲区大小
 *   Parameter: sockd_fd -> socket 描述符
 *              size -> 发送缓冲区大小
 *      Return: 0 -> 设置失败。
 *              1 -> 设置成功 。
 */
int sock_set_snd_buffer_size(int sock_fd, int size);

/**
 *        Name: is_valid_ip
 * Description: 是否是合法 IP
 *   Parameter: ip_str -> 是否是合法 IP
 *      Return: 0 -> 不是合法 IP。
 *              1 -> 合法 IP。
 */
int is_valid_ip(const char *ip_str);

/**
 *        Name: set_reuse_addr
 * Description: 设置地址重用
 *   Parameter: sock_fd -> sock 描述符
 *      Return: 0 -> 设置失败
 *              1 -> 设置成功
 */
int set_reuse_addr(int sock_fd);
/**
 *        Name: get_ip
 * Description: 获取本机的 IP
 *   Parameter: version -> v4 | v6.
 *	 Parameter: address ->
 *	 				char address[INET_ADDRSTRLEN] |
 *	 				char address[INET6_ADDRSTRLEN]
 *      Return: 1 -> 成功
 *      		0 -> 失败
  */
int get_ip(const char* version, char* address);
#endif //CLIBRARIES_NETWORK_H
