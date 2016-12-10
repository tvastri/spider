#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <curl/curl.h>
#include "global.h"
#include "config.h"
#include "server.h"
#include "debug.h"

static tClientStats clientStats;

void
initClientStats()
{
    memset(&clientStats, 0, sizeof(clientStats));
}

void
incClientStatsNstat()
{
    clientStats.nstat++;
}

void
incClientStatsNupld()
{
    clientStats.nupld++;
}

tClientStats*
get_client_stats(tClientStats *cs)
{
    *cs = clientStats;
    return cs;
}

static char *
get_reg_url(char *buf)
{
    strcpy(buf, "http://");
    strcat(buf, get_client_config()->ipaddr);
    strcat(buf, ":");
    strcat(buf, SPIDER_SERVER_PORT);
    strcat(buf, "/reg/");
    return buf;
}

char *
get_stat_url(char *buf)
{
    strcpy(buf, "http://");
    strcat(buf, get_client_config()->ipaddr);
    strcat(buf, ":");
    strcat(buf, SPIDER_SERVER_PORT);
    strcat(buf, "/stat/");
    return buf;
}

 char *
get_get_url(char *buf)
{
    strcpy(buf, "http://");
    strcat(buf, get_client_config()->ipaddr);
    strcat(buf, ":");
    strcat(buf, SPIDER_SERVER_PORT);
    strcat(buf, "/get/");
    return buf;
}

static char *
get_upload_url(char *buf)
{
    strcpy(buf, "http://");
    strcat(buf, get_client_config()->ipaddr);
    strcat(buf, ":");
    strcat(buf, SPIDER_SERVER_PORT);
    strcat(buf, "/upld/");
    return buf;
}

void
net_init()
{
    curl_global_init(CURL_GLOBAL_ALL);
}

static size_t
stat_file_write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    char *buf = (char *)userdata;

    memcpy(buf, ptr, size*nmemb);
    printf("%s called ...size = %lu, nmemb = %lu, buf = %s\n", __FUNCTION__, size, nmemb, buf);
    return size*nmemb;
}

static tStatus
stat_file(char *file, uint32_t *status)
{
    long http_code = 0;
    CURL *curl;
    CURLcode res;
    char stat_url[1024] = {0,};
    char resp_buffer[512] = {0, };

    strcpy(stat_url, get_stat_url(stat_url));
    strcat(stat_url, "?file=");
    strcat(stat_url, file);

    printf("stat_url = %s\n", stat_url);
    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_PROXY, "");
        curl_easy_setopt(curl, CURLOPT_URL, stat_url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, stat_file_write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)resp_buffer);

        res = curl_easy_perform(curl);
        curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
        if (res == CURLE_OK && http_code == 200)
        {
            printf("http_code1 = %ld\n", http_code);
            *status = atoi(resp_buffer);
            goto success;
        }
        else
        {
            printf("Stat failed. http_code = %ld\n", http_code);
            goto err;
        }
    }

success:
    curl_easy_cleanup(curl);
    return OK;

err:
    curl_easy_cleanup(curl);
    return ERROR;
}

tBoolean
file_present_on_server(char *file)
{
    int     backoff=1;
    uint32_t http_status;
    tStatus ret;

    do
    {
        ret = stat_file(file, &http_status);
        if (OK != ret)
        {
            backoff = (backoff > 4000)?4096:2*backoff;
            printf("Backing off for %d secs for file %s\n", backoff, file);
            sleep(backoff);
        }
    } while(OK != ret);

    incClientStatsNstat();
    printf("http_status == %u\n", http_status);
    return (http_status==FILE_PRESENT)?TRUE:FALSE;
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
    char  url_buf[1024];
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
        curl_easy_setopt(curl, CURLOPT_URL, get_upload_url(url_buf));
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

    incClientStatsNupld();

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
    CURL                    *curl;
    CURLcode                  res;
    char            url_buf[1024];

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
        curl_easy_setopt(curl, CURLOPT_URL, get_reg_url(url_buf));
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            debug_log(LOG_ERR, "GET of server config failed");
            return ERROR;
        }
        curl_easy_cleanup(curl);
    }

    return decode_server_config(t);
}
