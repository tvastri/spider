#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <time.h>
#include <errno.h>
#include "global.h"
#include "debug.h"
#include "file_utils.h"
#include "net_utils.h"
#include "scan_dir.h"
#include "config_utils.h"

tStatus
change_user_and_root(char *uid)
{
    struct passwd      pwd;
    struct passwd    *ppwd;
    char         *buf=NULL;
    char        *root=NULL;
    int           buflen=0;
    int           status=0;

    if ( 0 > (buflen = sysconf(_SC_GETPW_R_SIZE_MAX)))
    {
        debug_log(LOG_CRIT, "sysconf failed.");
        return ERROR;
    }

    buf = malloc(buflen);
    if (NULL == buf)
    {
        debug_log(LOG_CRIT, "malloc failed.");
        return ERROR;
    }

    status = getpwnam_r(uid, &pwd, buf, buflen, &ppwd);
    if (0 > status)
    {
        debug_log(LOG_CRIT, "getpwnam_r failed.");
        return ERROR;
    }

    if (0 == getuid())
    {
        /* Change uid, gid and root if runnign as root */
        if (0 > chroot(pwd.pw_dir))
        {
            debug_log(LOG_CRIT, "chroot failed.");
            return ERROR;
        }

        if (getgid() != pwd.pw_gid)
        {
            if (setgid(pwd.pw_gid))
            {
                debug_log(LOG_CRIT, "setgid failed.");
                return ERROR;
            }
        }

        if (getuid() != pwd.pw_uid)
        {
            if (setuid(pwd.pw_uid))
            {
                debug_log(LOG_CRIT, "setuid failed.");
                return ERROR;
            }
        }
    }
    else
    {
        if (!(root = getenv("HOME")))
        {
            debug_log(LOG_CRIT, "getenv failed.");
            return ERROR;
        }
        
        if (0 > chdir(root))
        {
            debug_log(LOG_CRIT, "chdir failed.");
            return ERROR;
        }
    }

    return OK;
}

static tStatus
spider_init(int daemonize, int create_scratchpad)
{
    config_init();

    /* Read the client config */
    if (OK != decode_client_config(CONFIG_FILE, get_client_config()))
    {
        debug_log(LOG_CRIT, "Could not read %s. Exiting.", CONFIG_FILE);
        return ERROR;
    }
 
    print_client_config();

    /* Change UID and GID and chroot */
    if (OK != change_user_and_root(get_client_config()->uid))
    {
        debug_log(LOG_CRIT, "Could not change uid and root. Exiting.");
        return ERROR;
    }

    /* Mount the scratchpad directory */
    if (create_scratchpad && (OK != mount_scratchpad(SCRATCHPAD_DIR, 512, getuid(), getgid())))
    {
        debug_log(LOG_CRIT, "Could not mount SCRATCHPAD_DIR.");
        return ERROR;
    }

    if (daemonize)
    {
        daemon(0, 0);       
    }

    debug_log_init(daemonize);

    net_init();

    srand(time(NULL));

    /* Create timestamp directory if not present */
    if (OK != create_cache_dir_if_missing(CACHE_DIR))
    {
        return ERROR;
    }

    /* Is scratchpad mounted */
    if (FALSE == scratchpad_is_tmpfs(SCRATCHPAD_DIR))
    {
        debug_log(LOG_CRIT, "Scratchpad dir is not tmpfs.");
        return ERROR;
    }

    debug_log(LOG_NOTICE, "Starting client.");

    return OK;
}

static void
spider_end()
{
    debug_log_close();
}

int
main(int argc, char *argv[])
{
    static int                   daemonize=FALSE;
    static int           create_scratchpad=FALSE;
    int                                        c;
    time_t                                   now;
    //time_t                      pscan_interval;
    time_t                   next_fscan_time = 0;
    //time_t                 next_pscan_time = 0;
    //time_t                last_fscan_timestamp;
    tFileData            fileData = {NULL, NULL};

    while(1)
    {
        static struct option long_options[] =
        {
            {"daemonize",          no_argument,               &daemonize, 1},
            {"create-scratchpad",  no_argument,       &create_scratchpad, 1},
            {"help",     no_argument,       0, 'h'},
            {0, 0, 0, 0}
        };

        int option_index = 0;

        c = getopt_long (argc, argv, "h", long_options, &option_index);

        if (c == -1)
            break;

        switch(c)
        {
            case 0:
                if (long_options[option_index].flag != 0)
                    break;
                if (optarg)
                    printf (" with arg %s", optarg);
                    printf ("\n");
                break;
            case 'h':
                printf("Print help1\n");
                break;

            case '?':
                break;

            default:
                printf("Print help2\n");
                exit(1);
        }

    }

    printf("daemonize = %d, create_scratchpad = %d\n", daemonize, create_scratchpad);

    if (OK != spider_init(daemonize, create_scratchpad))
    {
        debug_log(LOG_CRIT, "Initialization failed.");
        exit(1);
    }


    printf("server ipaddr = %s\n", get_client_config()->ipaddr);


    // The main loop
    while(1)
    {
        //sleep(LOOP_INTERVAL);

        if (OK != download_config_from_server(get_server_config()))
        {
            /* Maybe the server is down */
            debug_log(LOG_NOTICE, "Server config download failed.");
            continue;
        }

        if (OK != file_data_init(&fileData, get_server_config()->max_file_size, get_server_config()->max_path_len))
        {
            /* Maybe the server is down */
            debug_log(LOG_NOTICE, "File data init failed.");
            continue;
        }

        now = time(&now);
        printf("now = %lu, next_fscan_time = %lu, get_fscan_interval = %lu\n", now, next_fscan_time, get_fscan_interval());

        if (now > next_fscan_time)
        {
            debug_log(LOG_NOTICE, "Starting full scan.");
            do_scan(SPIDER_FULL_SCAN, &fileData, ".", 0, get_client_config()->backoff_interval);
            debug_log(LOG_NOTICE, "Full scan completed in %u seconds.", time(NULL) - now);
            next_fscan_time = time(NULL) + (get_fscan_interval()/10)*9 + rand()%(get_fscan_interval()/10);
            printf("now = %lu, next_fscan_time = %lu\n", now, next_fscan_time);
            store_last_fscan_timestamp(now);
        }
        exit(1);
        sleep(600);
#if 0
        else if (now > next_pscan_time)
        {
            if (ERROR == get_last_fscan_timestamp(&last_fscan_timestamp))
            {
                debug_log(LOG_ERR, "Could not read last timestamp. Skipping partial scan.");
                continue;
            }
            do_pscan(SPIDER_PARTIAL_SCAN, &fileData, root, last_fscan_timestamp, get_client_config()->backoff_interval);
            debug_log(LOG_NOTICE, "Partial scan completed in %u seconds.", time(NULL) - now);
            next_pscan_time = time(NULL) + (pscan_interval*9)/10 + rand()%(pscan_interval/10);
        }
        else
        {
            // Do nothing
        }
#endif
    }

    spider_end();

    return 0;
}
