#ifndef _NET_UTILS_H
#define _NET_UTILS_H

#include "global.h"
#include "config_utils.h"

void net_init();
tBoolean hash_present_on_server();
tStatus upload_file(char *file);
tStatus download_config_from_server(tServerConfig *t);


#endif
