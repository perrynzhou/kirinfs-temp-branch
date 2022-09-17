/*************************************************************************
    > File Name: c_log.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Mon 28 Mar 2022 12:12:43 PM UTC
 ************************************************************************/

#ifndef _C_LOG_H
#define _C_LOG_H
 #include "c_utils.h"
#define	LOG_ERR     0 
#define	LOG_WARNING 1
#define	LOG_INFO    2 
#define	LOG_DEBUG   3  


typedef enum {
    C_LOG_STDIN   = 0, 
    C_LOG_STDOUT  = 1, 
    C_LOG_STDERR  = 2, 
    C_LOG_FILE    = 3,
} c_log_type_t;

int c_log_init(c_log_type_t type,const char *file);
void c_log_set_level(int level);
void c_log_deinit();
int c_log_print(int lvl,  const char *file, int line,const char *func, const char *fmt, ...);
#define loge(...) c_log_print(LOG_ERR,  __FILE__, __LINE__, __func__, __VA_ARGS__)
#define logw(...) c_log_print(LOG_WARNING, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define logi(...) c_log_print(LOG_INFO, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define logd(...) c_log_print(LOG_DEBUG, __FILE__, __LINE__, __func__, __VA_ARGS__)

#endif
