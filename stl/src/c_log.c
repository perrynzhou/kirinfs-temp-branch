/*************************************************************************
    > File Name: c_log.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Mon 28 Mar 2022 12:12:58 PM UTC
 ************************************************************************/

#include <stdio.h>
#include "inc/c_log.h"

#if defined (__WIN32__) || defined (WIN32) || defined (_MSC_VER)
#define B_RED(str)      str
#define B_GREEN(str)    str
#define B_YELLOW(str)   str
#define B_BLUE(str)     str
#define B_MAGENTA(str)  str
#define B_CYAN(str)     str
#define B_WHITE(str)    str
#define RED(str)        str
#define GREEN(str)      str
#define YELLOW(str)     str
#define BLUE(str)       str
#define MAGENTA(str)    str
#define CYAN(str)       str
#define WHITE(str)      str
#else
#define FG_BLACK        30
#define FG_RED          31
#define FG_GREEN        32
#define FG_YELLOW       33
#define FG_BLUE         34
#define FG_MAGENTA      35
#define FG_CYAN         36
#define FG_WHITE        37
#define BG_BLACK        40
#define BG_RED          41
#define BG_GREEN        42
#define BG_YELLOW       43
#define BG_BLUE         44
#define BG_MAGENTA      45
#define BG_CYAN         46
#define BG_WHITE        47
#define B_RED(str)      "\033[1;31m" str "\033[0m"
#define B_GREEN(str)    "\033[1;32m" str "\033[0m"
#define B_YELLOW(str)   "\033[1;33m" str "\033[0m"
#define B_BLUE(str)     "\033[1;34m" str "\033[0m"
#define B_MAGENTA(str)  "\033[1;35m" str "\033[0m"
#define B_CYAN(str)     "\033[1;36m" str "\033[0m"
#define B_WHITE(str)    "\033[1;37m" str "\033[0m"
#define RED(str)        "\033[31m" str "\033[0m"
#define GREEN(str)      "\033[32m" str "\033[0m"
#define YELLOW(str)     "\033[33m" str "\033[0m"
#define BLUE(str)       "\033[34m" str "\033[0m"
#define MAGENTA(str)    "\033[35m" str "\033[0m"
#define CYAN(str)       "\033[36m" str "\033[0m"
#define WHITE(str)      "\033[37m" str "\033[0m"
#endif

#define USE_SYSLOG
#define LOG_IOVEC_MAX (10)
#define FILENAME_LEN (256)
#define FILESIZE_LEN (10 * 1024 * 1024UL)
#define LOG_BUF_SIZE (1024)
#define LOG_TIME_SIZE (32)
#define LOG_LEVEL_SIZE (32)
#define LOG_TAG_SIZE (32)
#define LOG_PNAME_SIZE (32)
#define LOG_TEXT_SIZE (256)
#define LOG_LEVEL_DEFAULT LOG_INFO
#define LOG_IO_OPS

/*
 *#define LOG_VERBOSE_ENABLE
 */

#define LOG_PREFIX_MASK (0xFFFF)
#define LOG_FUNCLINE_BIT (1 << 0)
#define LOG_TID_BIT (1 << 1)
#define LOG_PID_BIT (1 << 2)
#define LOG_TIMESTAMP_BIT (1 << 3)
#define LOG_TAG_BIT (1 << 4)

typedef struct
{
    char *log_file;
    int fd;
    size_t rorate_size;
    int log_level;
    pthread_mutex_t c_log_mutex;
    bool is_init;
} c_log_t;
static c_log_t g_log;
pthread_once_t thread_once = PTHREAD_ONCE_INIT;

/* from /usr/include/sys/syslog.h */
static const char *c_log_level_str[] = {
    "ERR",
    "WARN",
    "INFO",
    "DEBUG",
};
int c_log_init(c_log_type_t type, const char *file)
{
    if (!g_log.is_init)
    {
        if (type == C_LOG_FILE && file != NULL)
        {
            g_log.log_file = strdup(file);
            g_log.fd = open(g_log.log_file, O_RDWR | O_APPEND, 0644);
            assert(g_log.fd != -1);
        }
        if (g_log.fd < 0)
        {
            g_log.fd = type;
        }
        g_log.log_level = LOG_INFO;
        return 0;
    }
    return -1;
}
void c_log_set_level(int level)
{
    if (g_log.is_init)
    {
        g_log.log_level = level;
    }
}
static void c_log_get_time(char *str, int len, int flag_name)
{
    char date_fmt[20];
    char date_ms[32];
    struct timeval tv;
    struct tm now_tm;
    int now_ms;
    time_t now_sec;
    gettimeofday(&tv, NULL);
    now_sec = tv.tv_sec;
    now_ms = tv.tv_usec / 1000;
    localtime_r(&now_sec, &now_tm);

    if (flag_name == 0)
    {
        strftime(date_fmt, 20, "%Y-%m-%d %H:%M:%S", &now_tm);
        snprintf(date_ms, sizeof(date_ms), "%03d", now_ms);
        snprintf(str, len, "[%s.%s]", date_fmt, date_ms);
    }
    else
    {
        strftime(date_fmt, 20, "%Y_%m_%d_%H_%M_%S", &now_tm);
        snprintf(date_ms, sizeof(date_ms), "%03d", now_ms);
        snprintf(str, len, "%s_%s.log", date_fmt, date_ms);
    }
}
static int c_log_write(int lvl, const char *file, int line,
                       const char *func, const char *msg)
{
    int ret = 0, i = 0;
    struct iovec vec[LOG_IOVEC_MAX];
    char s_time[LOG_TIME_SIZE];
    char s_lvl[LOG_LEVEL_SIZE];
    // char s_pname[LOG_PNAME_SIZE*2];
    char s_pid[LOG_PNAME_SIZE];
    char s_file[LOG_TEXT_SIZE];
    char s_msg[LOG_BUF_SIZE];

    pthread_mutex_lock(&g_log.c_log_mutex);
    c_log_get_time(s_time, sizeof(s_time), 0);

    switch (lvl)
    {
    case LOG_ERR:
        snprintf(s_lvl, sizeof(s_lvl),
                 B_RED("[%7s]"), c_log_level_str[lvl]);
        snprintf(s_msg, sizeof(s_msg), RED("%s"), msg);
        break;
    case LOG_WARNING:
        snprintf(s_lvl, sizeof(s_lvl),
                 B_YELLOW("[%7s]"), c_log_level_str[lvl]);
        snprintf(s_msg, sizeof(s_msg), YELLOW("%s"), msg);
        break;
    case LOG_INFO:
        snprintf(s_lvl, sizeof(s_lvl),
                 B_GREEN("[%7s]"), c_log_level_str[lvl]);
        snprintf(s_msg, sizeof(s_msg), GREEN("%s"), msg);
        break;
    case LOG_DEBUG:
        snprintf(s_lvl, sizeof(s_lvl),
                 B_WHITE("[%7s]"), c_log_level_str[lvl]);
        snprintf(s_msg, sizeof(s_msg), WHITE("%s"), msg);
        break;
    default:
        snprintf(s_lvl, sizeof(s_lvl),
                 "[%7s]", c_log_level_str[lvl]);
        snprintf(s_msg, sizeof(s_msg), "%s", msg);
        break;
    }

    snprintf(s_pid, sizeof(s_pid), "[pid:%d]", getpid());
    snprintf(s_file, sizeof(s_file), "[%s:%3d: %s] \n", file, line, func);

    i = -1;
    vec[++i].iov_base = (void *)s_time;
    vec[i].iov_len = strlen(s_time);

    vec[++i].iov_base = (void *)s_pid;
    vec[i].iov_len = strlen(s_pid);

    vec[++i].iov_base = (void *)s_lvl;
    vec[i].iov_len = strlen(s_lvl);

    vec[++i].iov_base = (void *)s_file;
    vec[i].iov_len = strlen(s_file);

    vec[++i].iov_base = (void *)s_msg;
    vec[i].iov_len = strlen(s_msg);
    writev(g_log.fd, (const struct iovec *)&vec, i+1);
    fdatasync(g_log.fd);
    pthread_mutex_unlock(&g_log.c_log_mutex);

    return ret;
}

int c_log_print(int lvl, const char *file, int line, const char *func, const char *fmt, ...)
{
    va_list ap;
    char buf[LOG_BUF_SIZE] = {0};
    int n, ret;

    if (!g_log.is_init)
    {
        c_log_set_level(lvl);
        c_log_init(0, NULL);
    }

    if (lvl <LOG_ERR || lvl > LOG_DEBUG)
    {
        return -1;
    }

    va_start(ap, fmt);
    n = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    if (n < 0)
    {
        fprintf(stderr, "vsnprintf errno:%d\n", errno);
        return -1;
    }
    ret = c_log_write(lvl, file, line, func, buf);

    return ret;
}
void c_log_deinit() {
    if(g_log.is_init) {
        close(g_log.fd);
        g_log.is_init=false;
    }
}