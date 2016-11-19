#include <stdio.h>
#include <string.h>
#include <glib-2.0/glib.h>
#include "global.h"
#include "config.h"

tStatus
get_server_ip(char *config_file, char *ipaddr)
{
    GKeyFile            *keyfile;
    GKeyFileFlags          flags;
    GError         *error = NULL;
    gchar               *lipaddr;

    keyfile = g_key_file_new ();
    flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

    if (!g_key_file_load_from_file (keyfile, config_file, flags, &error))
    {
        g_error (error->message);
        return ERROR;
    }

    lipaddr=g_key_file_get_string(keyfile,"Server","ipaddr",NULL);

    strcpy(ipaddr, lipaddr);

    return OK;
}
