#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <glib-2.0/glib.h>
#include "global.h"
#include "debug.h"
#include "config.h"

static tClientConfig clientConfig;
static tServerConfig serverConfig;

void
config_init()
{
    memset(&serverConfig, 0, sizeof(serverConfig));
    memset(&clientConfig, 0, sizeof(clientConfig));
}

tStatus
decode_client_config(char *config_file, tClientConfig *client)
{
    GKeyFile                   *keyfile;
    GKeyFileFlags                 flags;
    GError                *error = NULL;
    gchar               *lipaddr = NULL;
    gchar                  *luid = NULL;
    gchar                *lemail = NULL;
    gchar      *backoff_interval = NULL;

    memset(&clientConfig, 0, sizeof(clientConfig));

    keyfile = g_key_file_new ();
    flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

    if (!g_key_file_load_from_file (keyfile, config_file, flags, &error))
    {
        g_error (error->message);
        return ERROR;
    }

    lipaddr=g_key_file_get_string(keyfile,"Server","ipaddr",&error);

    if (lipaddr == NULL)
    {
        debug_log(LOG_CRIT, "Could not decode server IP address");
        return ERROR;
    }

    luid=g_key_file_get_string(keyfile,"User","uid",&error);

    if (luid == NULL)
    {
        debug_log(LOG_CRIT, "Could not decode UID");
        return ERROR;
    }

    lemail=g_key_file_get_string(keyfile,"User","email",&error);

    if (lemail == NULL)
    {
        debug_log(LOG_CRIT, "Could not decode email ID");
        return ERROR;
    }

    backoff_interval=g_key_file_get_string(keyfile,"Scan","backoff_interval",&error);

    if (lemail == NULL)
    {
        debug_log(LOG_CRIT, "Could not decode backoff_interval");
        return ERROR;
    }

    strcpy(client->ipaddr, lipaddr);
    strcpy(client->uid, luid);
    strcpy(client->email, lemail);
    client->backoff_interval = atol(backoff_interval);

    return OK;
}

tClientConfig*
get_client_config()
{
    return &clientConfig;
}

tServerConfig*
get_server_config()
{
    return &serverConfig;
}

static tBoolean
check_file_extn(char *file, char *extn)
{
    size_t slen = strlen (file);
    size_t elen = strlen (extn);

    if (slen < elen)
        return FALSE;

    return (strcmp (&(file[slen-elen]), extn) == 0);
}

tBoolean
backup_worthy(char *file, struct stat *file_stat)
{
    int i=0;

    while(serverConfig.ignore_extn[i][0])
    {
        if (TRUE == check_file_extn(file, &serverConfig.ignore_extn[i][0]))
            return FALSE;
        i++;
    }

    return TRUE;
}

tStatus
decode_ignored_extensions(tServerConfig *server, char *ignore_extn)
{
    int i=0;
    char *token;

    token = strtok(ignore_extn, CONFIG_LIST_SEPARATOR);

    while(NULL != token )
    {
        strcpy(&server->ignore_extn[i][0], token);
        i++;
        token = strtok(NULL, CONFIG_LIST_SEPARATOR);
    }
 
    return OK;
}

tStatus
decode_server_config(tServerConfig *server)
{
    GKeyFile               *keyfile;
    GKeyFileFlags             flags;
    GError            *error = NULL;
    gchar       *full_scan_interval;
    gchar    *partial_scan_interval;
    gchar            *max_file_size;
    gchar             *max_path_len;
    gchar              *ignore_extn;

    keyfile = g_key_file_new ();
    flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

    if (!g_key_file_load_from_data(keyfile, server->scratchpad.data, server->scratchpad.size, flags, &error))
    {
        g_error (error->message);
        return ERROR;
    }

    full_scan_interval=g_key_file_get_string(keyfile,"Scan","full_scan_interval",NULL);
    partial_scan_interval=g_key_file_get_string(keyfile,"Scan","partial_scan_interval",NULL);
    max_file_size=g_key_file_get_string(keyfile,"Scan","max_file_size",NULL);
    max_path_len=g_key_file_get_string(keyfile,"Scan","max_path_len",NULL);
    ignore_extn=g_key_file_get_string(keyfile,"Scan","ignore_exensions",NULL);

    if (full_scan_interval == NULL)
    {
        debug_log(LOG_CRIT, "Could not decode full scan interval");
        return ERROR;
    }
    if (partial_scan_interval == NULL)
    {
        debug_log(LOG_CRIT, "Could not decode partial scan interval");
        return ERROR;
    }
    if (max_file_size == NULL)
    {
        debug_log(LOG_CRIT, "Could not decode max_file_size");
        return ERROR;
    }
    if (max_path_len == NULL)
    {
        debug_log(LOG_CRIT, "Could not decode max_path_len");
        return ERROR;
    }
    if (ignore_extn == NULL)
    {
        debug_log(LOG_CRIT, "Could not decode ignore_extn");
        return ERROR;
    }

    server->fscan_interval = atol(full_scan_interval);
    server->pscan_interval = atol(partial_scan_interval);
    server->max_file_size = atol(max_file_size);
    server->max_path_len = atol(max_path_len);

    decode_ignored_extensions(server, ignore_extn);

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

void
print_client_config()
{
    printf("[Server]\n");
    printf("ipaddr = %s\n", clientConfig.ipaddr);
    printf("[User]\n");
    printf("uid = %s\n", clientConfig.uid);
    printf("email = %s\n", clientConfig.email);

}
