#include <stdio.h>
#include <curl/curl.h>
#include "global.h"

tBoolean
hash_present_on_server()
{
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl)
    {

        curl_easy_cleanup(curl);
    }

    return FALSE;
}
