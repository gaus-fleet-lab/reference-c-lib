//The MIT License (MIT)
//
//Copyright 2018, Sony Mobile Communications Inc.
//
//Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#include <gaus/gaus_client_types.h>
#include "curl_wrapper.h"
#include "gaus.h"
#include "gaus/gaus_client.h"
#include "log.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdarg.h>

gaus_global_state_t gaus_global_state = {
    NULL,   //Server
    false,  //Initialized
    NULL    //Proxy
};

gaus_version_t gaus_client_library_version(void) {
  gaus_version_t version = {0, 0, 2};
  return version;
}

gaus_error_t *gaus_global_init(const char *serverUrl, const gaus_initialization_options_t *options) {
  if (!gaus_global_state.globalInitalized) {
    //Set state:
    CURLcode status = gaus_curl_global_init(CURL_GLOBAL_ALL);
    if (status != CURLE_OK) {
      return gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500, "Failed to globally initialize curl");
    }
    gaus_global_state.serverUrl = strdup(serverUrl);
    if (options && options->proxy) {
      gaus_global_state.proxy = strdup(options->proxy);
    } else {
      //Ensure that proxy is initialized to NULL if not set.
      gaus_global_state.proxy = NULL;
    }
    if (options && options->ca_path) {
      gaus_global_state.ca_path = strdup(options->ca_path);
    } else {
      //Ensure that ca_path is initialized to NULL if not set.
      gaus_global_state.ca_path = NULL;
    }
    gaus_global_state.globalInitalized = true;

  }
  return NULL;
}

void gaus_global_cleanup(void) {
  if (gaus_global_state.globalInitalized) {
    free(gaus_global_state.proxy);
    free(gaus_global_state.ca_path);
    free(gaus_global_state.serverUrl);
    gaus_curl_global_cleanup();
    gaus_global_state.globalInitalized = false;
  }
}

gaus_error_t *
gaus_create_error(const char *func, gaus_error_type_t type, unsigned int code, const char *description, ...) {

  //Figure out how big of a string is needed and malloc to fill in description.
  va_list args;
  va_start(args, description);
  va_list args_backup;  //vsnprintf mangles args so we need a backup
  va_copy(args_backup, args);
  int needed = vsnprintf(NULL, 0, description, args) + 1;
  char *buffer = malloc((size_t) needed);
  vsprintf(buffer, description, args_backup);
  va_end(args);
  va_end(args_backup);
  gaus_error_t *error = (gaus_error_t *) malloc(sizeof(gaus_error_t));

  logging(L_ERROR, "%s: %s", func, buffer);
  error->error_type = type;
  error->http_error_code = code;
  error->description = buffer;
  return error;
}
