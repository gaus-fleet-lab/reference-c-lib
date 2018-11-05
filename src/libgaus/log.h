//The MIT License (MIT)
//
//Copyright 2018, Sony Mobile Communications Inc.
//
//Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#ifndef GAUS_UPDATECLIENT_LOG_H
#define GAUS_UPDATECLIENT_LOG_H

#define L_DEBUG 0
#define L_INFO 1
#define L_NOTICE 2
#define L_WARNING 3
#define L_ERROR 4

#define L_RAW (1 << 10) /* Modifier to log without timestamp */

void init_logging(void);

void logging(int level, const char *fmt, ...);

void set_loglevel(int level);

void set_loglevel_from_string(char *loglevel);

#endif
