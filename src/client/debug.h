#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>


#define DEBUG_LEVEL_1         1
#define DEBUG_LEVEL_2         2
#define DEBUG_LEVEL_3         3

#define DEBUG_LEVEL_CONSOLE   0x1
#define DEBUG_LEVEL_SYSLOG    0x2

extern void debug_log(uint32_t level, uint32_t dest, char *fmt, ...);
