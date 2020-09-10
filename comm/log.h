//
// Created by hwp on 2017/11/8.
//

#ifndef CLIBRARIES_LIBLOG_H
#define CLIBRARIES_LIBLOG_H

#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO  1
#define LOG_LEVEL_WARN  2
#define LOG_LEVEL_ERROR 3

/**
 *        Name: log_set_file
 * Description: set log file.
 *   Parameter: file_name -> 日志文件.
 *      Return: 1 -> success
 *              0 -> failed.
 */
int log_set_file(char *file_name);

/**
 *        Name: log_set_file
 * Description: set log file.
 *   Parameter: file_name -> 日志文件.
 *      Return: 1 -> success
 *              0 -> failed.
 */
int log_set_syslog(char *ip, int port, int facility, char *hostname, char *program);

/**
 *        Name: log_close
 * Description: close log.
 *   Parameter:
 *      Return: 1 -> success
 *              0 -> failed.
 */
int log_close();

/**
 *        Name: log_set_level
 * Description: set log level.
 *   Parameter: level -> log level.
 *      Return:
 */
void log_set_level(int level);

/**
 *        Name: log_by_level
 * Description: output log by log_level.
 *   Parameter: level -> log level;
 *              fmt   -> input format string similar to printf
 *      Return: 1 -> success.
 *              0 -> failed.
 */
int log_by_level(int level, const char *fmt, ...);

/**
 *        Name: log_debug
 * Description: output log by DEBUG level.
 *   Parameter: fmt -> input format string similar to printf.
 *      Return: 1 -> success.
 *              0 -> failed.
 */
int log_debug(const char *fmt, ...);

/**
 *        Name: log_info
 * Description: output log by INFO level.
 *   Parameter: fmt -> input format string similar to printf.
 *      Return: 1 -> success.
 *              0 -> failed.
 */
int log_info(const char *fmt, ...);

/**
 *        Name: log_warn
 * Description: output log by WARN level.
 *   Parameter: fmt -> input format string similar to printf.
 *      Return: 1 -> success.
 *              0 -> failed.
 */
int log_warn(const char *fmt, ...);

/**
 *        Name: log_error
 * Description: output log by ERROR level.
 *   Parameter: fmt -> input format string similar to printf.
 *      Return: 1 -> success.
 *              0 -> failed.
 */
int log_error(const char *fmt, ...);

/**
 *        Name: log_level_str
 * Description: return log level str description by log level.
 *   Parameter: log_level -> log level.
 *      Return: return log level str description by log level.
 */
char *log_level_str(int log_level);

/**
 *        Name: log_level_int
 * Description: return log level int by log level str.
 *   Parameter: log_level_str -> log level string description.
 *      Return: return log level int.
 *              -1 -> error.
 */
int log_level_int(char *log_level_str);
#endif //CLIBRARIES_LIBLOG_H
