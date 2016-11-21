#ifndef _GLOBAL_H
#define _GLOBAL_H

#define CACHE_DIR       ".spider"
#define TIMESTAMP_FILE  CACHE_DIR "/" "last_timestamp"
#define CONFIG_FILE     "/etc/spider.cfg"
#define IP_ADDR_LEN     16

typedef u_int8_t tBoolean;
#if (1 != TRUE)
#define TRUE    1
#endif

#ifdef FALSE
#if (0 != FALSE)
#define FALSE   0
#endif
#else
#define FALSE   0
#endif

typedef u_int8_t tStatus;
#define OK      1
#define ERROR   0

typedef struct
{
    u_int32_t  fscan_interval;
    u_int32_t  pscan_interval;
} tServerConfig;
#endif /* _CONFIG_H */
