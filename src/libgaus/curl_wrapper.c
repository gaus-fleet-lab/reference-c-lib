#include "curl_wrapper.h"

//Initialize all of "our" gaus_curl_easy to use curl_easy by default
//This is to allow overriding in testing.
curl_global_init_t *gaus_curl_global_init = curl_global_init;
curl_easy_perform_t *gaus_curl_easy_perform = curl_easy_perform;
curl_easy_init_t *gaus_curl_easy_init = curl_easy_init;
curl_easy_setopt_t *gaus_curl_easy_setopt = curl_easy_setopt;
curl_easy_cleanup_t *gaus_curl_easy_cleanup = curl_easy_cleanup;
curl_global_cleanup_t *gaus_curl_global_cleanup = curl_global_cleanup;
curl_easy_getinfo_t *gaus_curl_easy_getinfo = curl_easy_getinfo;
