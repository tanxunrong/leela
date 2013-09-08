
#include "myc.h"
#include <curl/curl.h>

int main(int argc,char *argv[])
{
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if(curl != NULL)
    {
        curl_easy_setopt(curl,CURLOPT_URL,"http://www.baidu.com");
        curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION,1L);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            fprintf(stderr,"perform failed : %s\n",curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);
    }

    return 0;
}
