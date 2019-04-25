#ifndef SIMPLE_DEBUG_COMMON_H_
#define SIMPLE_DEBUG_COMMON_H_

typedef enum {
    LOG_DBG = 0,
    LOG_INFO,
    LOG_ERR
} log_e;

static char *m_debug_info[] = {"DBG","INFO","ERROR"};
static log_e m_level = LOG_INFO;

#define LOG(level, format, ...) \
    do { \
        if (level >= m_level) { \
            fprintf(stderr, "[%s|%s@%s,%d] " format "\n", \
                    m_debug_info[level], __func__, __FILE__, __LINE__, ##__VA_ARGS__ ); \
        } \
    } while (0)

#endif
