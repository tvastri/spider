#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <curl/curl.h>
#include "global.h"
#include "debug.h"

typedef struct
{
    int      size;
    char    *data;
} tServerConfigData;

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
    tServerConfigData *pServData;

    pServData = (tServerConfigData *)userp;

    pServData->data = (char *)realloc(pServData->data, pServData->size + size*nmemb);
    if (NULL == pServData->data)
    {
        debug_log(LOG_CRIT, "malloc failed. errno = %s", strerror(errno));
        return 0;
    }

    memcpy(pServData->data+pServData->size, buffer, size*nmemb); 
    pServData->size += size*nmemb;

    return size*nmemb;
}

tBoolean
get_config_from_server(tServerConfig *t)
{
    CURL             *curl;
    CURLcode           res;
    tServerConfigData data = {0, NULL};

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_PROXY, "");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, store_server_config); 
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data); 
        curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:8000/conf/");
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            fprintf(stderr, "Get failed for conf.");
        }
        else
        {
            fprintf(stderr, "Get succeeded for conf.");
        }
        curl_easy_cleanup(curl);
    }

    return OK;
}
