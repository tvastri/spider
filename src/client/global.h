#ifndef _GLOBAL_H
#define _GLOBAL_H

#define CACHE_DIR          ".spider/"
#define SCRATCHPAD_DIR     ".spider/scratchpad/"
#define TIMESTAMP_FILE     CACHE_DIR "/" "last_timestamp"
#define CONFIG_FILE        "/etc/spider/client.cfg"
#define IP_ADDR_LEN        16
#define MAX_NAME_LEN       256
#define LOOP_INTERVAL      10
#define MAX_IGNORE_EXTN    16
#define IGNORE_EXTN_SIZE   16

#define CONFIG_LIST_SEPARATOR ","

#define SPIDER_SERVER_PORT "8500"

/* Server status codes */

#define FILE_PRESENT          100
#define FILE_NOT_PRESENT      101
#define FILE_UPLOADED         110
#define FILE_WRITE_FAILED     111
#define DIR_CREAT_FAILED      113
#define FILE_OPEN_FAILED      115
#define WRONG_METHOD          151


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

#endif /* _CONFIG_H */
