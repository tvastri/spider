#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <linux/magic.h>
#include <openssl/sha.h>
#include <string.h>
#include <errno.h>
#include <sys/mount.h>
#include "debug.h"
#include "file_utils.h"

#define TS_BUF               16
#define MOUNT_OPTIONS_SIZE  128

tStatus
file_data_init(tFileData *fData, uint32_t max_size, uint32_t max_name_len)
{
    fData->content = realloc(fData->content, max_size);
    if (NULL == fData->content)
    {
        debug_log(LOG_ERR, "realloc failed for content.");
        return ERROR;
    }

    fData->name = realloc(fData->name, max_name_len);
    if (NULL == fData->name)
    {
        debug_log(LOG_ERR, "realloc failed for name.");
        return ERROR;
    }

    return OK;
}

tBoolean
scratchpad_is_tmpfs(char *scratchpad_dir)
{
    struct statfs  fsprop;

    if (statfs(scratchpad_dir, &fsprop) < 0)
    {
        debug_log(LOG_CRIT, "scratchpad dir not mounted.");
        return FALSE;
    }

    if (TMPFS_MAGIC != fsprop.f_type)
    {
        return FALSE;
    }

    return TRUE;
}

tStatus
mount_scratchpad(char *scratchpad_dir, uint32_t size_mb, uid_t uid, gid_t gid)
{
    int result;
    const char* src  = "none";
    const char* trgt = "/mnt";
    const char* type = "tmpfs";
    const unsigned long mntflags = 0;
    char opts[MOUNT_OPTIONS_SIZE+1];   /* 65534 is the uid of nobody */

    snprintf(opts, MOUNT_OPTIONS_SIZE, "mode=0700,uid=%u,gid=%u,size=%uM", uid, gid, size_mb);

    result = mount(src, trgt, type, mntflags, opts);

    if (result == 0)
    {
        debug_log(LOG_CRIT, "Failed to create tmpfs at %s.", scratchpad_dir);
        return ERROR;
    }
    return OK;
}

tStatus
create_cache_dir_if_missing(char *cache_dir)
{
    struct stat buf;

    if (lstat(cache_dir, &buf) < 0)
    {
        debug_log(LOG_ERR, "lstat failed for file %s. errno = %s", cache_dir, strerror(errno));
        if (0 == mkdir(cache_dir, 0700))
        {
            return OK;
        }
        else
        {
            debug_log(LOG_ERR, " mkdir failed for dir %s. errno = %s", cache_dir, strerror(errno));
            return ERROR;
        }
    }

    if (!S_ISDIR(buf.st_mode))
    {
        debug_log(LOG_ERR, "%s is not a directory\n", cache_dir);
        return ERROR;
    }

    return OK;
}

tStatus
calculate_sha1_hash(char *infile, unsigned char *hash)
{
    tStatus                           ret;
    struct                      stat fbuf;
    unsigned char           *file_content;
    FILE                               *f;

    if (0 > stat(infile, &fbuf))
    {
        debug_log(LOG_ERR, "stat failed for file %s. errno = %s", infile, strerror(errno));
        exit(1);
    }

    file_content = (unsigned char *)malloc(fbuf.st_size);
    if (file_content == NULL)
    {
        debug_log(LOG_ERR, "malloc failed. errno = %s", strerror(errno));
        exit(1);
    }

    f = fopen(infile, "rb");
    if (f == NULL)
    {
        debug_log(LOG_ERR, "fopen failed. errno = %s", strerror(errno));
        exit(1);
    }

    ret = fread(file_content, fbuf.st_size, 1, f);
    if (1 == ret)
    {
        SHA1(file_content, fbuf.st_size , hash);
    }

    return OK;
}

tStatus
store_last_fscan_timestamp(time_t timestamp)
{
    FILE                           *ts;

    if ((ts = fopen(TIMESTAMP_FILE, "w")))
    {
        fprintf(ts, "%lu", timestamp);
        fclose(ts);
    }
    else
    {
        debug_log(LOG_ERR, "Could not write timestamp file.");
    }

    return OK;
}

tStatus
get_last_fscan_timestamp(time_t *timestamp)
{
    FILE                           *ts;
    char    last_timestamp_buf[TS_BUF];

    if ((ts = fopen(TIMESTAMP_FILE, "r")))
    {
        fgets(last_timestamp_buf, TS_BUF, ts);
        *timestamp = atoi(last_timestamp_buf);
        fclose(ts);
    }
    else
    {
         *timestamp = 0;
         return ERROR;
    }

    return OK;
}

tStatus
backup_file(tFileData *fileData)
{
    /* Calculate SHA1 */
    //calculate_sha1_hash(fileData->name, fileData->sha1);
    //printf("%s %s\n", fileData->name, fileData->sha1);
    printf("%s\n", fileData->name);
    return OK;
}

