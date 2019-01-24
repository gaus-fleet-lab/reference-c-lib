//The MIT License (MIT)
//
//Copyright 2018, Sony Mobile Communications Inc.
//
//Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#ifndef GAUS_GAUS_H
#define GAUS_GAUS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <gaus/gaus_client_types.h>

typedef struct {
  char *serverUrl;
  bool globalInitalized;
  char *proxy;
  char *ca_path;
} gaus_global_state_t;

extern gaus_global_state_t gaus_global_state;

gaus_error_t *
gaus_create_error(const char *func, gaus_error_type_t type, unsigned int code, const char *description, ...);


#ifdef __cplusplus
}
#endif
#endif //GAUS_GAUS_H
