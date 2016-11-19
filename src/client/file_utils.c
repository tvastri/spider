#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <openssl/sha.h>
#include <errno.h>
#include "file_utils.h"

tStatus
create_cache_dir_if_missing(char *cache_dir)
{
    struct stat buf;

    if (lstat(cache_dir, &buf) < 0)
    {
        fprintf(stderr, "%s: lstat failed (%d)\n", __FUNCTION__, errno);
        if (0 == mkdir(cache_dir, 0700))
        {
            return OK;
        }
        else
        {
            fprintf(stderr, "%s: mkdir failed (%d)\n", __FUNCTION__, errno);
            return ERROR;
        }
    }

    if (!S_ISDIR(buf.st_mode))
    {
        fprintf(stderr, "%s: %s is not a directory\n", __FUNCTION__, cache_dir);
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
        fprintf(stderr, "stat failed %d\n", errno);
        exit(1);
    }

    file_content = (unsigned char *)malloc(fbuf.st_size);
    if (file_content == NULL)
    {
        fprintf(stderr, "malloc failed %d\n", errno);
        exit(1);
    }

    f = fopen(infile, "rb");
    if (f == NULL)
    {
        fprintf(stderr, "fopen failed %d\n", errno);
        exit(1);
    }

    ret = fread(file_content, fbuf.st_size, 1, f);
    if (1 == ret)
    {
        SHA1(file_content, fbuf.st_size , hash);
    }

    return OK;
}
