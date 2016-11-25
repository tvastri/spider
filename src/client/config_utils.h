#ifndef _CONFIG_UTILS_H
#define _CONFIG_UTILS_H

#include "global.h"

typedef struct
{
    struct
    {
        int      size;
        char    *data;
    } scratchpad;
    time_t  fscan_interval;
    time_t  pscan_interval;
} tServerConfig;

typedef struct
{
    char           ipaddr[IP_ADDR_LEN];
    char             uid[MAX_NAME_LEN];
    char           email[MAX_NAME_LEN];
} tClientConfig;

void config_init();
tStatus decode_client_config(char *config_file, tClientConfig *clientConfig);
tStatus decode_server_config(tServerConfig *serverConfig);

tClientConfig* get_client_config();
tServerConfig* get_server_config();
tStatus get_server_ip(char *config_file, char *ipaddr);
time_t get_fscan_interval();
time_t get_pscan_interval();
void print_client_config();

#endif
