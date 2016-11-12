#include <stdio.h>
#include <stdlib.h>
#include "debug.h"

void
debug_log(uint32_t level, uint32_t dest, char *fmt, ...)
{
    va_list args;
    char buffer[BUFSIZ];

    va_start(args,fmt);
    vsprintf(buffer, fmt, args);
    fprintf(stderr, buffer);
    fprintf(stderr, "\n");
    va_end(args);

}
