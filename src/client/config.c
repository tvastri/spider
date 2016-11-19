#include <stdio.h>
#include <glib-2.0/glib.h>
#include "global.h"
#include "config.h"

tStatus
get_server_ip(char *ipaddr)
{
    GKeyFile            *keyfile;
    GKeyFileFlags          flags;
    GError *error = NULL;

    keyfile = g_key_file_new ();
    flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

    if (!g_key_file_load_from_file (keyfile, CONFIG_FILE, flags, &error))
    {
        g_error (error->message);
        return ERROR;
    }
    return OK;
}
