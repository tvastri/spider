#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib-2.0/glib.h>
#include "global.h"
#include "config_utils.h"

static tServerConfig serverConfig;
static tClientConfig clientConfig;

void
config_init()
{
    memset(&serverConfig, 0, sizeof(serverConfig));
    memset(&clientConfig, 0, sizeof(clientConfig));
}

tStatus
decode_client_config(char *config_file)
{
    GKeyFile            *keyfile;
    GKeyFileFlags          flags;
    GError         *error = NULL;
    gchar               *lipaddr;

    memset(&clientConfig, 0, sizeof(clientConfig));

    keyfile = g_key_file_new ();
    flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

    if (!g_key_file_load_from_file (keyfile, config_file, flags, &error))
    {
        g_error (error->message);
        return ERROR;
    }

    lipaddr=g_key_file_get_string(keyfile,"Server","ipaddr",NULL);

    strcpy(clientConfig.ipaddr, lipaddr);

    return OK;
}

tServerConfig*
get_server_config()
{
    return &serverConfig;
}

tStatus
decode_server_config()
{
    GKeyFile               *keyfile;
    GKeyFileFlags             flags;
    GError            *error = NULL;
    gchar       *full_scan_interval;
    gchar    *partial_scan_interval;

    keyfile = g_key_file_new ();
    flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

    if (!g_key_file_load_from_data(keyfile, serverConfig.scratchpad.data, serverConfig.scratchpad.size, flags, &error))
    {
        g_error (error->message);
        return ERROR;
    }

    full_scan_interval=g_key_file_get_string(keyfile,"Scan","full_scan_interval",NULL);
    partial_scan_interval=g_key_file_get_string(keyfile,"Scan","partial_scan_interval",NULL);

    serverConfig.fscan_interval = atol(full_scan_interval);
    serverConfig.pscan_interval = atol(partial_scan_interval);

    printf("%s: %s %s\n", __FUNCTION__, full_scan_interval, partial_scan_interval);
    return OK;
}

time_t
get_fscan_interval()
{
    return serverConfig.fscan_interval;
}

time_t
get_pscan_interval()
{
    return serverConfig.pscan_interval;
}
