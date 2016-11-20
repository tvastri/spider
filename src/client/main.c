#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include "global.h"
#include "debug.h"
#include "file_utils.h"
#include "net_utils.h"
#include "scan_dir.h"

#define TS_BUF  16

static void
spider_init(tBoolean daemonize)
{
    debug_log_init(daemonize);

    debug_log(LOG_NOTICE, "Starting client.");

    if (daemonize)
    {
        daemon(0, 0);       
    }
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
    FILE                           *ts;
    time_t              last_timestamp;
    time_t           current_timestamp;
    char    last_timestamp_buf[TS_BUF];
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

    /* chdir to HOME directory */
    if (root = getenv("HOME"))
    {
        chdir(root);
    }
    else
    {
        debug_log(LOG_CRIT, "Environmental variable HOME not configured.");
        exit(1);
    }

    /* Get the server IP address */
    if (ERROR == get_server_ip(CONFIG_FILE, ipaddr))
    {
        debug_log(LOG_CRIT, "Could not read server from %s.", CONFIG_FILE);
        exit(1);
    }
    printf("server ipaddr = %s\n", ipaddr);

    /* Get the current timestamp */
    current_timestamp = time(&current_timestamp);

    /* Get the last time stamp */
    if (ts = fopen(TIMESTAMP_FILE, "r"))
    {
        fgets(last_timestamp_buf, TS_BUF, ts);
        last_timestamp = atoi(last_timestamp_buf);
        fclose(ts);
    }
    else
    {
         last_timestamp = 0;
    }

    printf("timestamp = %u, current_timestamp = %u\n", last_timestamp, current_timestamp);

    //scan_dir(root, last_timestamp);

    /* Create timestamp directory if not present */
    if (OK != create_cache_dir_if_missing(CACHE_DIR))
    {
        debug_log(LOG_CRIT, "Could not create cache directory.");
        exit(1);
    }

    /* store the timestamp */
    if (ts = fopen(TIMESTAMP_FILE, "w"))
    {
        fprintf(ts, "%u", current_timestamp);
        fclose(ts);
    }
    else
    {
        debug_log(LOG_ERR, "Could not write timestamp file.");
    }

    spider_end();

    return 0;
}
