#ifndef _SCAN_DIR_H
#define _SCAN_DIR_H

#include <time.h>

tStatus do_fscan(char *root_dir);
tStatus do_pscan(char *root_dir, time_t last_timestamp);

#endif /* _SCAN_DIR_H */
