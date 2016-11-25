#ifndef _FILE_UTILS_H
#define _FILE_UTILS_H

#include <openssl/sha.h>
#include "global.h"

typedef struct
{
    unsigned char *content;
    char *name;
    char sha1[SHA_DIGEST_LENGTH];
} tFileData;

tStatus file_data_init(tFileData *fData, uint32_t max_size, uint32_t max_name_len);
tBoolean scratchpad_is_tmpfs(char *cratchpad_dir);
tStatus mount_scratchpad(char *scratchpad_dir, uint32_t size_mb, uid_t uid, gid_t gid);
tStatus create_cache_dir_if_missing(char *cache_dir);
tStatus store_last_fscan_timestamp(time_t timestamp);
tStatus get_last_fscan_timestamp(time_t *timestamp);
tStatus backup_file(tFileData *fileData);

#endif /* _FILE_UTILS_H */
