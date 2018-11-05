#ifndef GAUS_GAUS_CURL_H
#define GAUS_GAUS_CURL_H

#include <curl/curl.h>

#ifdef __cplusplus
extern "C" {
#endif

//Allow for mocking of curl functions in testing.
typedef CURLcode(curl_global_init_t)(long flags);
typedef CURLcode(curl_easy_perform_t)(CURL *curl);
typedef CURL *(curl_easy_init_t)(void);
typedef CURLcode (curl_easy_setopt_t)(CURL *curl, CURLoption option, ...);
typedef void (curl_easy_cleanup_t)(CURL *curl);
typedef void (curl_global_cleanup_t)(void);
typedef CURLcode (curl_easy_getinfo_t)(CURL *curl, CURLINFO info, ...);

extern curl_global_init_t *gaus_curl_global_init;
extern curl_easy_perform_t *gaus_curl_easy_perform;
extern curl_easy_init_t *gaus_curl_easy_init;
extern curl_easy_setopt_t *gaus_curl_easy_setopt;
extern curl_easy_cleanup_t *gaus_curl_easy_cleanup;
extern curl_global_cleanup_t *gaus_curl_global_cleanup;
extern curl_easy_getinfo_t *gaus_curl_easy_getinfo;

#ifdef __cplusplus
}
#endif
#endif //GAUS_GAUS_CURL_H
