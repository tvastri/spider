#include <stdio.h>
#include <curl/curl.h>
#include "global.h"

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

tBoolean
get_config_from_server(tServerConfig *t)
{
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_PROXY, "");
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
