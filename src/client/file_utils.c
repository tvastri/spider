#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <openssl/sha.h>
#include <errno.h>
#include "debug.h"
#include "file_utils.h"

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
calculate_sha1_hash(char *infile)
{
    tStatus                           ret;
    struct                      stat fbuf;
    unsigned char           *file_content;
    FILE                               *f;
    unsigned char hash[SHA_DIGEST_LENGTH]; // == 20


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
