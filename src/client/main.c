#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include "config.h"
#include "debug.h"
#include "scan_dir.h"

#define TS_BUF  16
int
main(int argc, char *argv[])
{
    int                              c;
    int                          debug;
    char                         *root;
    FILE                           *ts;
    time_t              last_timestamp;
    time_t           current_timestamp;
    char    last_timestamp_buf[TS_BUF];

    while ((c = getopt (argc, argv, "d:")) != -1)
    {
        switch(c)
        {
            case 'd':
                debug = atoi(optarg);
                break;
            case '?':
                if (optopt == 'c')
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint(optopt))
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
                return 1;
            default:
                exit(1);
        }
    }

    /* chdir to HOME directory */
    if (root = getenv("HOME"))
    {
        chdir(root);
    }
    else
    {
        debug_log(DEBUG_LEVEL_1, DEBUG_LEVEL_SYSLOG, "Environmental variable HOME not configured");
        exit(1);
    }

    /* Get the current timestamp */
    current_timestamp = time(&current_timestamp);

    /* Get the last time stamp */
    if (ts = fopen(TIMESTAMP_FILE, "r+"))
    {
        fgets(last_timestamp_buf, TS_BUF, ts);
        last_timestamp = atoi(last_timestamp_buf);
    }
    else
    {
         last_timestamp = 0;
    }

    printf("timestamp = %u, current_timestamp = %u\n", last_timestamp, current_timestamp);

    //scan_dir(root, last_timestamp);

    /* store the timestamp */
    fseek(ts, 0L, SEEK_SET);
    fprintf(ts, "%u", current_timestamp);

    fclose(ts);

    return 0;
}
