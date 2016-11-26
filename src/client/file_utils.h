#ifndef _FILE_UTILS_H
#define _FILE_UTILS_H

#include <stdint.h>
#include <openssl/sha.h>
#include "global.h"

typedef struct
{
    unsigned char *content;
    uint32_t size;
    char *name;
    unsigned char sha1hash[SHA_DIGEST_LENGTH];
    char hashname[2*SHA_DIGEST_LENGTH+4+1]; //  hash.spd
} tFileData;

tStatus file_data_init(tFileData *fData, uint32_t max_size, uint32_t max_name_len);
tBoolean scratchpad_is_tmpfs(char *cratchpad_dir);
tStatus mount_scratchpad(char *scratchpad_dir, uint32_t size_mb, uid_t uid, gid_t gid);
tStatus create_cache_dir_if_missing(char *cache_dir);
tStatus store_last_fscan_timestamp(time_t timestamp);
tStatus get_last_fscan_timestamp(time_t *timestamp);
tStatus backup_file(FILE *index_fp, tFileData *fileData);

#endif /* _FILE_UTILS_H */
