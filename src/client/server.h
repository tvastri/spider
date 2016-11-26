#ifndef _NET_UTILS_H
#define _NET_UTILS_H

#include "global.h"
#include "config.h"

void net_init();
tBoolean file_present_on_server(char *file);
tStatus upload_file(char *file);
tStatus download_config_from_server(tServerConfig *t);


#endif
