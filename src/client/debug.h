#ifndef _DEBUG_H
#define _DEBUG_H

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <syslog.h>
#include "global.h"


#define DEBUG_LEVEL_1         1
#define DEBUG_LEVEL_2         2
#define DEBUG_LEVEL_3         3

#define DEBUG_LEVEL_CONSOLE   0x1
#define DEBUG_LEVEL_SYSLOG    0x2

void debug_log_init(tBoolean console);
void debug_log(uint32_t prio, char *fmt, ...);
void debug_log_close(void);

#endif
