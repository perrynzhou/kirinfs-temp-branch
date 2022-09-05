#ifndef STL_LOG_H
#define STL_LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>

typedef struct {
  va_list ap;
  const char *fmt;
  const char *file;
  struct tm *time;
  void *udata;
  int line;
  int level;
} stl_log_event;

typedef void (*stl_log_logfn)(stl_log_event *ev);
typedef void (*stl_log_lockfn)(bool lock, void *udata);

enum { STL_LOG_TRACE, STL_LOG_DEBUG, STL_LOG_INFO, STL_LOG_WARN, STL_LOG_ERROR, STL_LOG_FATAL };

#define stl_log_trace(...) stl_log_write(STL_LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define stl_log_debug(...) stl_log_write(STL_LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define stl_log_info(...)  stl_log_write(STL_LOG_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define stl_log_warn(...)  stl_log_write(STL_LOG_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define stl_log_error(...) stl_log_write(STL_LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define stl_log_fatal(...) stl_log_write(STL_LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

void stl_log_set_lock(stl_log_lockfn fn, void *udata);
void stl_log_set_level(int level);
void stl_log_set_quiet(bool enable);
int stl_log_add_callback(stl_log_logfn fn, void *udata, int level);
int stl_log_add_fp(FILE *fp, int level);

void stl_log_write(int level, const char *file, int line, const char *fmt, ...);

#endif
