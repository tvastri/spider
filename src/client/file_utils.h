#ifndef _FILE_UTILS_H
#define _FILE_UTILS_H

#include "global.h"

tBoolean scratchpad_is_tmpfs(char *cratchpad_dir);
tStatus mount_scratchpad(char *scratchpad_dir, uint32_t size_mb, uid_t uid, gid_t gid);
tStatus create_cache_dir_if_missing(char *cache_dir);
tStatus store_last_fscan_timestamp(time_t timestamp);
tStatus get_last_fscan_timestamp(time_t *timestamp);




#endif /* _FILE_UTILS_H */
