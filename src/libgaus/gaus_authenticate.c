//The MIT License (MIT)
//
//Copyright 2018, Sony Mobile Communications Inc.
//
//Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#include "gaus/gaus_client.h"
#include "curl_wrapper.h"
#include "gaus.h"
#include "log.h"
#include "request.h"
#include "gaus_json_helpers.h"
#include "../include/gaus/gaus_client_types.h"

#include <jansson.h>
#include <string.h>

//Helper functions
static gaus_error_t *
parse_authenticate_json(json_t *root, gaus_session_t *session);

gaus_error_t *gaus_authenticate(const char *device_access, const char *device_secret, gaus_session_t *session) {
  gaus_error_t *status = NULL;
  char *raw_authenticate_result = NULL;
  char *json_auth_post_string = NULL;
  json_t *json_authenticate_body = NULL;
  json_t *json_authenticate_response = NULL;

  if (!gaus_global_state.globalInitalized) {
    status = gaus_create_error(__func__, GAUS_NO_INIT_ERROR, 500, "Authenticated without initializing");
    goto error;
  }

  if (!device_access || !device_secret || !session) {
    status = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500, "Authenticated invalid parameters");
    goto error;
  }

  //Ensure that all char * pointers in session are initialized to NULL so they can be freed safely
  session->device_guid = NULL;
  session->product_guid = NULL;
  session->token = NULL;

  json_authenticate_body = json_pack("{s:{s:s, s:s}}",
                                     DEVICE_AUTH_PARAM_JSON,
                                     ACCESS_KEY_JSON, device_access,
                                     SECRET_KEY_JSON, device_secret
  );

  json_auth_post_string = json_dumps(json_authenticate_body, JSON_COMPACT);

  char url[256];
  create_url(url, sizeof(url), "%s/authenticate", gaus_global_state.serverUrl);
  long status_code = 200; //Initialize to a default passing value unless request says otherwise.
  raw_authenticate_result = request_post_as_string(url, NULL, json_auth_post_string, &status_code);
  if (!raw_authenticate_result && status_code < 400) {
    status = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500, "Posting authenticate failed");
    goto error;
  }
  if (status_code >= 400) {
    status = gaus_create_error(__func__, GAUS_HTTP_ERROR, status_code,
                               "Posting authenticate failed with http error code %d",
                               status_code);
    goto error;
  }

  json_error_t json_error;
  if (!(json_authenticate_response = json_loads(raw_authenticate_result, JSON_DECODE_ANY, &json_error))) {
    status = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500, "Error parsing json ");
    goto error;
  }

  status = parse_authenticate_json(json_authenticate_response, session);

  error:
  free(raw_authenticate_result);
  free(json_auth_post_string);
  json_decref(json_authenticate_body);
  json_decref(json_authenticate_response);

  return status;
}

static gaus_error_t *parse_authenticate_json(json_t *root, gaus_session_t *session) {
  gaus_error_t *error = NULL;

  if (!json_is_object(root)) {
    error = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500, "Server reply invalid: json root is not an object");
    goto error;
  }

  if (!(session->device_guid = dup_dict_string(root, DEVICE_GUID_JSON, NULL))) {
    error = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500,
                              "Server reply invalid: required \"deviceGUID\" missing in object");
    goto error;
  }

  if (!(session->product_guid = dup_dict_string(root, PRODUCT_GUID_JSON, NULL))) {
    error = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500,
                              "Server reply invalid: required \"productGUID\" missing in object");
    goto error;
  }

  if (!(session->token = dup_dict_string(root, TOKEN_JSON, NULL))) {
    error = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500,
                              "Server reply invalid: required \"token\" missing in object");
    goto error;
  }

  error:
  return error;
}
