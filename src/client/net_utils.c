#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <curl/curl.h>
#include "global.h"
#include "config_utils.h"
#include "debug.h"

tBoolean
hash_present_on_server()
{
    CURL *curl;
    //CURLcode res;

    curl = curl_easy_init();
    if (curl)
    {

        curl_easy_cleanup(curl);
    }

    return FALSE;
}

size_t
store_server_config(void *buffer, size_t size, size_t nmemb, void *userp)
{
    tServerConfig *serverConfig;

    serverConfig = (tServerConfig *)userp;

    serverConfig->scratchpad.data = (char *)realloc(serverConfig->scratchpad.data, serverConfig->scratchpad.size + size*nmemb);
    if (NULL == serverConfig->scratchpad.data)
    {
        debug_log(LOG_CRIT, "malloc failed. errno = %s", strerror(errno));
        return 0;
    }

    memcpy(serverConfig->scratchpad.data+serverConfig->scratchpad.size, buffer, size*nmemb); 
    serverConfig->scratchpad.size += size*nmemb;

    return size*nmemb;
}

tStatus
download_config_from_server(tServerConfig *t)
{
    CURL             *curl;
    CURLcode           res;

    /* Initialize the server config structure */
    if (t->scratchpad.size)
    {
        free(t->scratchpad.data);
        t->scratchpad.data = NULL;
    }

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_PROXY, "");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, store_server_config); 
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, t); 
        curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:8000/conf/");
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            return ERROR;
        }
        curl_easy_cleanup(curl);
    }

    return decode_server_config();
}
