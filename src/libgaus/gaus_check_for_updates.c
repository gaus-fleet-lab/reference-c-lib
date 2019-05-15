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

static gaus_error_t *parse_update_json(json_t *root, unsigned int *updateCount, gaus_update_t **updates);

gaus_error_t *
gaus_check_for_updates(const gaus_session_t *session, unsigned int filter_count, const gaus_header_filter_t *filters,
                       unsigned int *update_count, gaus_update_t **updates) {
  gaus_error_t *status = NULL;
  char *raw_check_for_update_result = NULL;
  json_t *json_update_response = NULL;
  char *query_parms = NULL;
  size_t required_length = 256;
  char* url = malloc(required_length);

  if (!gaus_global_state.globalInitalized) {
    status = gaus_create_error(__func__, GAUS_NO_INIT_ERROR, 500, "Checked for updates without initializing");
    goto error;
  }

  if (!session || !session->device_guid || !session->product_guid || !session->token
      || !update_count || !updates) {
    status = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500, "Check for updates with invalid parameters");
    goto error;
  }

  if (filter_count > 0 && !filters) {
    status = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500, "Check for updates with invalid parameters");
    goto error;
  }

  if (filter_count > 0) {
    query_parms = strdup("?");
  } else {
    query_parms = strdup("");
  }
  for (unsigned int i = 0; i < filter_count; i++) {
    //Create strings for each filter:
    char *filter_string = strdup(i > 0 ? "&%s=%s" : "%s=%s");
    int required_length = snprintf(NULL, 0, filter_string, filters[i].filter_name, filters[i].filter_value) + 1;
    char *new_filter = malloc(required_length);
    sprintf(new_filter, filter_string, filters[i].filter_name, filters[i].filter_value);
    query_parms = realloc(query_parms, strlen(query_parms) + required_length);
    strcat(query_parms, new_filter);
    free(filter_string);
    free(new_filter);
  }

  long status_code = 200; //Initialize to a default passing value unless request says otherwise.

  //Fixme: This should be fixed for production
  int url_length = create_url(url, required_length, "%s/device/%s/%s/check-for-updates%s",
                            gaus_global_state.serverUrl, session->product_guid, session->device_guid, query_parms);

  while(url_length < 0) {
      free(url);
      required_length += 256;
      url = malloc(required_length);
      url_length = create_url(url, required_length, "%s/device/%s/%s/check-for-updates%s",
                 gaus_global_state.serverUrl, session->product_guid, session->device_guid, query_parms);
  }

  raw_check_for_update_result = request_get_as_string(url, session->token, &status_code);
  if (!raw_check_for_update_result && status_code < 400) {
    status = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500, "Posting authenticate failed to url %s", url);
    goto error;
  }
  if (status_code >= 400) {
    status = gaus_create_error(__func__, GAUS_HTTP_ERROR, status_code,
                               "Posting register failed with http error code %d to url %s",
                               status_code, url);
    goto error;
  }

  json_error_t json_error;
  if (!(json_update_response = json_loads(raw_check_for_update_result, JSON_DECODE_ANY, &json_error))) {
    status = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500, "Error parsing json ");
    goto error;
  }

  status = parse_update_json(json_update_response, update_count, updates);


  error:
  free(url);
  free(query_parms);
  free(raw_check_for_update_result);
  json_decref(json_update_response);
  return status;
}

static gaus_error_t *parse_update_json(json_t *root, unsigned int *updateCount, gaus_update_t **updates) {
  gaus_error_t *error = NULL;
  json_t *json_updates = NULL;

  if (!json_is_object(root)) {
    error = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500, "Server reply invalid: json root is not an object");
    goto error;
  }

  json_updates = json_object_get(root, UPDATES_JSON);
  if (!json_is_array(json_updates)) {
    error = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500,
                              "Server reply invalid: \"deviceAuthParameters\" was not an object");
    goto error;
  }

  *updateCount = json_array_size(json_updates);

  if (*updateCount > 0) {
    //Allocate memory for updates:
    *updates = malloc(sizeof(gaus_update_t) * *updateCount);

    for (size_t i = 0; i < *updateCount; i++) {
      json_t *json_metadata = NULL;

      json_t *json_current_update = json_array_get(json_updates, i);
      //Ensure that updates has valid to free etc:

      (*updates)[i].metadata = NULL;
      (*updates)[i].metadata_count = 0;
      (*updates)[i].size = 0;
      (*updates)[i].update_type = NULL;
      (*updates)[i].package_type = NULL;
      (*updates)[i].md5 = NULL;
      (*updates)[i].update_id = NULL;
      (*updates)[i].version = NULL;
      (*updates)[i].download_url = NULL;

      json_metadata = json_object_get(json_current_update, METADATA_JSON);
      if (!json_is_object(json_metadata)) {
        error = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500,
                                  "Server reply invalid: \"metadata\" was not an object");
        goto error;
      }

      (*updates)[i].metadata_count = json_object_size(json_metadata);

      //Allocate memory for metadata:
      (*updates)[i].metadata = malloc(sizeof(gaus_key_value_t) * (*updates)[i].metadata_count);

      const char *key = NULL;
      json_t *value = NULL;
      unsigned int j = 0;
      json_object_foreach(json_metadata, key, value) {
        (*updates)[i].metadata[j].key = strdup(key);

        if (!json_is_string(value)) {
          error = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500,
                                    "Server reply invalid: metadata value is not a string");
          goto error;
        }

        (*updates)[i].metadata[j].value = strdup(json_string_value(value));
        j++;
      }

      if (!((*updates)[i].update_type = dup_dict_string(json_current_update, UPDATE_TYPE_JSON, NULL))) {
        error = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500,
                                  "Server reply invalid: required \\\"updateType\\\" missing in object\"");
        goto error;
      }

      if (!((*updates)[i].package_type = dup_dict_string(json_current_update, PACKAGE_TYPE_JSON, NULL))) {
        error = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500,
                                  "Server reply invalid: required \\\"packageType\\\" missing in object\"");
        goto error;
      }

      if (!((*updates)[i].update_id = dup_dict_string(json_current_update, UPDATE_ID_JSON, NULL))) {
        error = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500,
                                  "Server reply invalid: required \\\"updateId\\\" missing in object\"");
        goto error;
      }

      if (!((*updates)[i].version = dup_dict_string(json_current_update, VERSION_JSON, NULL))) {
        error = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500,
                                  "Server reply invalid: required \\\"version\\\" missing in object\"");
        goto error;
      }

      if (0 != strcmp((*updates)[i].package_type, PACKAGE_TYPE_FILE_JSON)) {
        logging(L_WARNING, "Received update of type \"%s\", not processing further.", (*updates)[i].package_type);
        (*updates)[i].size = 0;
        (*updates)[i].md5 = NULL;
        (*updates)[i].download_url = NULL;
        goto error;
      } else {
        if (!((*updates)[i].size = get_dict_int(json_current_update, SIZE_JSON, 0))) {
          error = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500,
                                    "Server reply invalid: required \"size\" missing in object");
          goto error;
        }

        if (!((*updates)[i].md5 = dup_dict_string(json_current_update, MD5_JSON, NULL))) {
          error = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500,
                                    "Server reply invalid: required \\\"md5\\\" missing in object\"");
          goto error;
        }

        if (!((*updates)[i].download_url = dup_dict_string(json_current_update, DOWNLOAD_URL_JSON, NULL))) {
          error = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500,
                                    "Server reply invalid: required \\\"downloadUrl\\\" missing in object\"");
          goto error;
        }
      }
    }
  }

  error:
  return error;
}
