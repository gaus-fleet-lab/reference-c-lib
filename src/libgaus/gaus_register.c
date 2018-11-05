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
#include <jansson.h>
#include <string.h>

//Helper functions
static gaus_error_t *
parse_device_json(json_t *root, char **device_access, char **device_secret, unsigned int *poll_interval_seconds);

gaus_error_t *gaus_register(const char *product_access, const char *product_secret, const char *device_id,
                            char **device_access, char **device_secret, unsigned int *poll_interval_seconds) {

  gaus_error_t *error = NULL;
  json_t *json_register_response = NULL;
  json_t *json_device_params = NULL;

  if (!gaus_global_state.globalInitalized) {
    return gaus_create_error(__func__, GAUS_NO_INIT_ERROR, 500, "Registered without initializing");
  }

  //Ensure inputs are valid-ish (not null)
  if (!product_secret || !product_access || !device_id
      || !device_access || !device_secret || !poll_interval_seconds) {
    return gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500, "Registered with invalid input parameters");
  }

  json_t *register_body_json = json_pack("{s:s,s:{s:s, s:s}}",
                                         DEVICE_ID_JSON, device_id,
                                         PRODUCT_AUTH_PARAM_JSON,
                                         ACCESS_KEY_JSON, product_access,
                                         SECRET_KEY_JSON, product_secret
  );

  char *jsonString = json_dumps(register_body_json, JSON_COMPACT);

  char url[256];
  create_url(url, sizeof(url), "%s/register", gaus_global_state.serverUrl);
  long status_code = 200; //Initialize to a default passing value unless request says otherwise.
  char *raw_register_result = request_post_as_string(url, NULL, jsonString, &status_code);
  if (!raw_register_result && status_code < 400) {
    error = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500, "Posting register failed");
    goto error;
  }
  if (status_code >= 400) {
    error = gaus_create_error(__func__, GAUS_HTTP_ERROR, status_code, "Posting register failed with http error code %d",
                              status_code);
    goto error;
  }

  //Fixme check json error
  json_error_t json_error;
  if (!(json_register_response = json_loads(raw_register_result, JSON_DECODE_ANY, &json_error))) {
    error = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500, "Error parsing json ");
    goto error;
  }

  error = parse_device_json(json_register_response, device_access, device_secret, poll_interval_seconds);

  error:
  free(raw_register_result);
  free(jsonString);
  json_decref(register_body_json);
  json_decref(json_device_params);
  json_decref(json_register_response);
  return error;
}

static gaus_error_t *
parse_device_json(json_t *root, char **device_access, char **device_secret, unsigned int *poll_interval_seconds) {
  json_t *json_device = NULL;
  gaus_error_t *error = NULL;

  if (!json_is_object(root)) {
    error = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500, "Server reply invalid: json root is not an object");
    goto error;
  }

  if (!(*poll_interval_seconds = get_dict_int(root, POLL_INTERVAL_JSON, 0))) {
    error = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500,
                              "Server reply invalid: required \"pollIntervalSeconds\" missing in object");
    goto error;
  }

  json_device = json_object_get(root, DEVICE_AUTH_PARAM_JSON);
  if (!json_is_object(json_device)) {
    error = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500,
                              "Server reply invalid: \"deviceAuthParameters\" was not an object");
    goto error;
  }

  if (!(*device_access = dup_dict_string(json_device, ACCESS_KEY_JSON, NULL))) {
    error = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500,
                              "Server reply invalid: required \\\"accessKey\\\" missing in object\"");
    goto error;
  }
  if (!(*device_secret = dup_dict_string(json_device, SECRET_KEY_JSON, NULL))) {
    error = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500,
                              "Server reply invalid: required \"secretKey\" missing in object");
    goto error;
  }
  error:
  return error;
}
