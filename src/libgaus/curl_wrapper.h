//The MIT License (MIT)
//
//Copyright 2018, Sony Mobile Communications Inc.
//
//Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
