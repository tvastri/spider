#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include "debug.h"
#include "global.h"

tBoolean log_console=FALSE;

void
debug_log_init(tBoolean daemon)
{
    openlog ("spider", 0, LOG_LOCAL1);
    log_console = (TRUE==daemon)?FALSE:TRUE;
}

void
debug_log(uint32_t prio, char *fmt, ...)
{
    va_list args;
    char buffer[BUFSIZ];

    va_start(args,fmt);
    vsprintf(buffer, fmt, args);
    syslog (prio, buffer);
    va_end(args);

    if (FALSE == log_console)
    {
        fprintf(stderr, buffer);
        fprintf(stderr, "\n");
    }
}

void
debug_log_close(void)
{
    closelog();
}
