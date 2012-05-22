#ifndef _H_LOG
#define _H_LOG

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

typedef uint32_t level_t;

#define MAX_MSG_LEN 1024

#define LOG_ERROR 40
#define LOG_WARNING 30
#define LOG_INFO 20
#define LOG_DEBUG 10

#define log_error(...) log(LOG_ERROR, __VA_ARGS__)
#define log_warn(...) log(LOG_WARNING, __VA_ARGS__)
#define log_info(...) log(LOG_INFO, __VA_ARGS__)
#define log_debug(...) log(LOG_DEBUG, __VA_ARGS__)

#define log(level, ...)\
	log_msg(level, 		__FILE__, __LINE__, 0, 0, __VA_ARGS__)
#define log_perror(...)\
	log_msg(LOG_ERROR, 	__FILE__, __LINE__, 1, 0, __VA_ARGS__)
#define log_herror(...)\
	log_msg(LOG_ERROR, 	__FILE__, __LINE__, 0, 1, __VA_ARGS__)


void log_msg(
	level_t level,
	const char *file,
	size_t linenum,
	int is_perror,
	int is_herror,
	const char *format, ...);

void log_configure(level_t level, FILE *stream);
#endif
