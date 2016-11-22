#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include "global.h"
#include "debug.h"
#include "file_utils.h"
#include "net_utils.h"
#include "scan_dir.h"
#include "config_utils.h"


static void
spider_init(tBoolean daemonize)
{

    config_init();

    if (daemonize)
    {
        daemon(0, 0);       
    }

    debug_log_init(daemonize);

    srand(time(NULL));

    /* Create timestamp directory if not present */
    if (OK != create_cache_dir_if_missing(CACHE_DIR))
    {
        debug_log(LOG_CRIT, "Could not create cache directory.");
        exit(1);
    }

    debug_log(LOG_NOTICE, "Starting client.");

}

static void
spider_end()
{
    debug_log_close();
}

int
main(int argc, char *argv[])
{
    int                              c;
    tBoolean           daemonize=FALSE;
    char                         *root;
    time_t                         now;
    //time_t              pscan_interval;
    time_t             next_fscan_time = 0;
    //time_t             next_pscan_time = 0;
    //time_t        last_fscan_timestamp;
    char           ipaddr[IP_ADDR_LEN] = {0};

    while ((c = getopt (argc, argv, "d")) != -1)
    {
        switch(c)
        {
            case 'd':
                daemonize = TRUE;
                spider_init(daemonize);
                break;
            case '?':
                if (optopt == 'c')
                    debug_log(LOG_CRIT, "Option -%c requires an argument.", optopt);
                else if (isprint(optopt))
                    debug_log(LOG_CRIT, "Unknown option `-%c'.", optopt);
                else
                    debug_log(LOG_CRIT, "Unknown option character `\\x%x'.", optopt);
                return 1;
            default:
                exit(1);
        }
    }

    spider_init(daemonize);

    if (OK != decode_client_config(CONFIG_FILE))
    {
        debug_log(LOG_CRIT, "Could not read %s. Exiting.", CONFIG_FILE);
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
        sleep(LOOP_INTERVAL);

        if (OK != download_config_from_server(get_server_config()))
        {
            /* Maybe the server is down */
            debug_log(LOG_NOTICE, "Server config download failed.");
            continue;
        }

        now = time(&now);
        printf("now = %lu, next_fscan_time = %lu, get_fscan_interval = %lu\n", now, next_fscan_time, get_fscan_interval());
        
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
