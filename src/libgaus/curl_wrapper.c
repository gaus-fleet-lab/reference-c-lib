//The MIT License (MIT)
//
//Copyright 2018, Sony Mobile Communications Inc.
//
//Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
