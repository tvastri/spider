#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>
#include "global.h"
#include "debug.h"
#include "file_utils.h"
#include "net_utils.h"
#include "scan_dir.h"
#include "config_utils.h"


static tStatus
spider_init(int daemonize, int create_scratchpad)
{

    config_init();

    /* Read the client config */
    if (OK != decode_client_config(CONFIG_FILE))
    {
        debug_log(LOG_CRIT, "Could not read %s. Exiting.", CONFIG_FILE);
        return ERROR;
    }
 
    /* Change UID and GID */

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
    char                                   *root;
    time_t                                   now;
    //time_t                      pscan_interval;
    time_t                   next_fscan_time = 0;
    //time_t                 next_pscan_time = 0;
    //time_t                last_fscan_timestamp;
    char               ipaddr[IP_ADDR_LEN] = {0};

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
    exit(1);

    if (OK != spider_init(daemonize, create_scratchpad))
    {
        debug_log(LOG_CRIT, "Initialization failed.");
        exit(1);
    }


    printf("server ipaddr = %s\n", ipaddr);

    /* chdir to HOME directory */
    if ((root = getenv("HOME")))
    {
        chdir(root);
    }
    else
    {
        debug_log(LOG_CRIT, "Environmental variable HOME not configured.");
        exit(1);
    }

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

        now = time(&now);
        printf("now = %lu, next_fscan_time = %lu, get_fscan_interval = %lu\n", now, next_fscan_time, get_fscan_interval());
        exit(1);
        if (now > next_fscan_time)
        {
            debug_log(LOG_NOTICE, "Starting full scan.");
            do_fscan(root);
            debug_log(LOG_NOTICE, "Full scan completed in %u seconds.", time(NULL) - now);
            next_fscan_time = time(NULL) + (get_fscan_interval()/10)*9 + rand()%(get_fscan_interval()/10);
            printf("now = %lu, next_fscan_time = %lu\n", now, next_fscan_time);
            store_last_fscan_timestamp(now);
        }
        sleep(600);
#if 0
        else if (now > next_pscan_time)
        {
            if (ERROR == get_last_fscan_timestamp(&last_fscan_timestamp))
            {
                debug_log(LOG_ERR, "Could not read last timestamp. Skipping partial scan.");
                continue;
            }
            do_pscan(root, last_fscan_timestamp);
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
