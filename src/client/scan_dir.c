#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <dirent.h>
#include <unistd.h>
#include <linux/magic.h>
#include <errno.h>
#include "debug.h"

#define MAX_PATH_LEN 1024

typedef struct _stack_node
{
    struct _stack_node            *next;
    DIR                           *pdir;
    char             path[MAX_PATH_LEN];
} t_stack_node;

t_stack_node *stack_head=NULL;

static void
stack_push(char *path, DIR *pdir)
{
    t_stack_node *n;

    n = malloc(sizeof(t_stack_node));   
    if (!n)
    {
        debug_log(LOG_ERR, "malloc failed. errno = %s", strerror(errno));
    }
    memset(n, 0, sizeof(*n));

    n->pdir = pdir;
    
    strcat(n->path, path);

    n->next = stack_head;
    stack_head = n;
}

static DIR *
stack_pop(char *path, DIR **ppdir)
{
    DIR     *pdir;
    t_stack_node *n;

    if (NULL == stack_head)
    {
        return NULL;
    }

    pdir = stack_head->pdir;
    *ppdir = stack_head->pdir;
    memcpy(path, stack_head->path, MAX_PATH_LEN);
    n = stack_head;
    stack_head = stack_head->next;
    free(n);

    return pdir;
}

int
scan_dir(char *root_dir, time_t last_timestamp)
{
    DIR            *pdir;
    DIR            *pndir;
    struct dirent *entry;
    struct stat    fprop;
    struct statfs  fsprop;
    char           current_dir[MAX_PATH_LEN] = {0};
    char           current_file[MAX_PATH_LEN] = {0};

    pdir = opendir(root_dir);

    if (NULL == pdir)
    {
        debug_log(LOG_CRIT, "opendir failed. errno = %s", strerror(errno));
        exit(1);
    }

    stack_push(".", pdir);

    while(pdir = stack_pop(current_dir, &pdir))
    {
        while(entry = readdir(pdir))
        {
            if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            {
                continue;
            }

            memset(current_file, 0, MAX_PATH_LEN);
            strcat(current_file, current_dir);
            strcat(current_file, "/");
            strcat(current_file, entry->d_name);

            if (lstat(current_file, &fprop) < 0)
            {
                debug_log(LOG_ERR, "lstat failed for %s. errno = %s", current_file, strerror(errno));
                exit(1);
            }
    
            if (S_ISREG(fprop.st_mode))
            {
                printf("%s\n", current_file);
            }
            else if (S_ISDIR(fprop.st_mode))
            {
                if (statfs(current_file, &fsprop) < 0)
                {
                    debug_log(LOG_ERR, "statfs failed for %s. errno = %s", current_file, strerror(errno));
                    exit(1);
                }

                if (fsprop.f_type == NFS_SUPER_MAGIC)
                {
                    continue;
                }

                /* A real directory */
                pndir = opendir(current_file);

                if (NULL == pndir)
                {
                    debug_log(LOG_ERR, "opendir failed for %s. errno = %s", current_file, strerror(errno));
                    exit(1);
                }
                stack_push(current_dir, pdir);
                stack_push(current_file, pndir);
                goto loop_break;     
            }
            else if (S_ISLNK(fprop.st_mode))
            {
                continue;
            }
            else
            {
                printf("UNK: ");
            }
        }
        closedir(pdir);
loop_break:
        memset(current_dir, 0, MAX_PATH_LEN);
    }

    closedir(pdir);
}
