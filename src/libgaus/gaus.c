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
  gaus_version_t version = {0, 0, 0};
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
    gaus_global_state.globalInitalized = true;

  }
  return NULL;
}

void gaus_global_cleanup(void) {
  if (gaus_global_state.globalInitalized) {
    free(gaus_global_state.proxy);
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
