#ifndef _NET_UTILS_H
#define _NET_UTILS_H

#include "global.h"
#include "config.h"

typedef struct tClientStats
{
    uint32_t nstat;
    uint32_t nupld;
} tClientStats;

void net_init();
tBoolean file_present_on_server(char *file);
tStatus upload_file(char *file);
tStatus download_config_from_server(tServerConfig *t);
tClientStats* get_client_stats(tClientStats *cs);


#endif
