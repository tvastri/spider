#ifndef _SCAN_DIR_H
#define _SCAN_DIR_H

#include <time.h>
#include "file_utils.h"

typedef enum
{
    SPIDER_FULL_SCAN,
    SPIDER_PARTIAL_SCAN
} eScanType;

tStatus do_scan(eScanType s, tFileData *fScratchpad, char *root_dir, time_t last_timestamp, uint32_t backoff_interval);

#endif /* _SCAN_DIR_H */
