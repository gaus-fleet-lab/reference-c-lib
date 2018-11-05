//The MIT License (MIT)
//
//Copyright 2018, Sony Mobile Communications Inc.
//
//Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#ifndef GAUS_UPDATECLIENT_REQUEST_H
#define GAUS_UPDATECLIENT_REQUEST_H

#include <stddef.h>

char *request_get_as_string(const char *url, const char *auth_token, long *status_code);

char *request_post_as_string(const char *url, const char *auth_token, const char *payload, long *status_code);

int request_get_as_file(const char *url, const char *token, int fd, long *status_code);

int create_url(char *dest, size_t dest_len, char *fmt, ...);

#endif
