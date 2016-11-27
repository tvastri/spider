#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <curl/curl.h>
#include "global.h"
#include "config.h"
#include "debug.h"

void
net_init()
{
    curl_global_init(CURL_GLOBAL_ALL);
}

tBoolean
file_present_on_server(char *file)
{
    long http_code = 0;
    CURL *curl;
    CURLcode res;
    char stat_url[1024] = {0,};

    strcpy(stat_url, SPIDER_STAT_URL);
    strcat(stat_url, "?file=");
    strcat(stat_url, file);

    printf("stat_url = %s\n", stat_url);
    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_PROXY, "");
        curl_easy_setopt(curl, CURLOPT_URL, stat_url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);

        res = curl_easy_perform(curl);
        if(res == CURLE_OK)
        {
            curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
            if (http_code == 200 && res != CURLE_ABORTED_BY_CALLBACK)
            {
                printf("http_code1 = %ld\n", http_code);
                curl_easy_cleanup(curl);
                return TRUE;
            }
            else
            {
                printf("http_code2 = %ld\n", http_code);
                curl_easy_cleanup(curl);
                return FALSE;
            }
        }

        curl_easy_cleanup(curl);
    }

    return FALSE;
}

static size_t
upload_file_write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    printf("%s called ...size = %lu, nmemb = %lu\n", __FUNCTION__, size, nmemb);
    return size*nmemb;
}

static tStatus
upload_file_curl(char *file)
{
    CURL *curl;
    CURLcode res;
    struct curl_httppost *formpost=NULL;
    struct curl_httppost *lastptr=NULL;
    struct curl_slist *headerlist=NULL;
    static const char buf[] = "Expect: 100-continue";

    curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "uploadfile", CURLFORM_FILE, file, CURLFORM_END);

    curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "submit", CURLFORM_COPYCONTENTS, "upload", CURLFORM_END);

    headerlist = curl_slist_append(headerlist, buf);

    curl = curl_easy_init();

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_PROXY, "");
        curl_easy_setopt(curl, CURLOPT_URL, SPIDER_UPLOAD_URL);
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, upload_file_write_callback);

        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
        {
            goto err;
        }

    }
    else
    {
        debug_log(LOG_ERR, "curl_easy_init() failed.\n");
        goto err;
    }

    curl_easy_cleanup(curl);
    curl_formfree(formpost);
    curl_slist_free_all (headerlist);

    return OK;

err:
    curl_easy_cleanup(curl);
    curl_formfree(formpost);
    curl_slist_free_all (headerlist);

    return ERROR;
}

tStatus
upload_file(char *file)
{
    int     backoff=1;
    tStatus       ret;

    do
    {
        ret = upload_file_curl(file);
        if (OK  != ret)
        {
            backoff = (backoff > 4000)?4096:2*backoff;
            printf("Backing off for %d secs for file %s\n", backoff, file);
            sleep(backoff);
        }
    } while(ret != OK);
    return OK;
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
        t->scratchpad.size = 0;
    }

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_PROXY, "");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, store_server_config); 
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, t); 
        curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:8000/reg/");
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            return ERROR;
        }
        curl_easy_cleanup(curl);
    }

    return decode_server_config(t);
}
