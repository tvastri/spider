#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
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

    if (S_IFDIR != buf.st_mode)
    {
        fprintf(stderr, "%s: %s is not a directory\n", __FUNCTION__, cache_dir);
        return ERROR;
    }

    return OK;
}

