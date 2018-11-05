#include "gaus/gaus_client.h"
#include "gaus.h"
#include "request.h"
#include "gaus_json_helpers.h"
#include "log.h"

#include <jansson.h>
#include <string.h>

static gaus_error_t *
get_json_for_vints(unsigned int int_count, gaus_v_int_t *v_ints, json_t **json_v_ints);

static gaus_error_t *
get_json_for_vfloats(unsigned int float_count, gaus_v_float_t *v_floats, json_t **json_v_floats);

static gaus_error_t *
get_json_for_vstrings(unsigned int string_count, gaus_v_string_t *v_strings, json_t **json_v_strings);

static gaus_error_t *create_json_for_header(const gaus_report_header_t *header, json_t **json_header);

static gaus_error_t *
create_json_for_update_status(const gaus_report_t *report, json_t **json_report);

static gaus_error_t *
create_json_for_generic(const gaus_report_t *report, json_t **json_report);

gaus_error_t *
gaus_report(const gaus_session_t *session, unsigned int filter_count, const gaus_header_filter_t *filters,
            const gaus_report_header_t *header, unsigned int report_count, const gaus_report_t *reports) {

  json_t *json_header = NULL;
  json_t *json_to_send = NULL;
  json_t *json_reports_array = NULL;
  json_t *json_temp_one_report = NULL;
  char *report_post_body = NULL;
  char *query_parms = NULL;

  gaus_error_t *status = NULL;
  char *raw_report_result = NULL;

  if (!gaus_global_state.globalInitalized) {
    status = gaus_create_error(__func__, GAUS_NO_INIT_ERROR, 500, "Checked for updates without initializing");
    goto error;
  }

  if (!session || !session->device_guid || !session->product_guid || !session->token
      || !header || report_count < 1 || !reports) {
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

  if (NULL != (status = create_json_for_header(header, &json_header))) {
    goto error;
  }

  if (NULL == (json_reports_array = json_array())) {
    status = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500, "Unable to create reports array");
    goto error;
  }

  //Process each report
  for (unsigned int i = 0; i < report_count; i++) {
    switch (reports[i].report_type) {
      case GAUS_REPORT_UPDATE:
        if (NULL != (status = create_json_for_update_status(&reports[i], &json_temp_one_report))) {
          goto error;
        }
        break;
      case GAUS_REPORT_GENERIC:
        if (NULL != (status = create_json_for_generic(&reports[i], &json_temp_one_report))) {
          goto error;
        }
        break;
      default:
        status = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500, "Unsupported report type!");
        goto error;
    }
    if (0 != json_array_append_new(json_reports_array, json_temp_one_report)) {
      status = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500, "Unable to add report to array");
      goto error;
    }

  }

  //Combine header/report array into what we will send:
  if (!(json_to_send = json_pack("{s:s,s:o,s:o}",
                                 VERSION_JSON, VERSION_1_0_0_JSON,
                                 HEADER_JSON, json_header,
                                 DATA_JSON, json_reports_array))) {
    status = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500, "Error encoding header");
  }

  report_post_body = json_dumps(json_to_send, JSON_COMPACT);

  //Fixme: This should be dynamically allocated:
  char url[256];
  create_url(url, sizeof(url), "%s/device/%s/%s/report%s",
             gaus_global_state.serverUrl, session->product_guid, session->device_guid, query_parms);
  long status_code = 200; //Initialize to a default passing value unless request says otherwise.
  raw_report_result = request_post_as_string(url, session->token, report_post_body, &status_code);
  if (!raw_report_result && status_code < 400) {
    status = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500, "Posting authenticate failed");
    goto error;
  }
  if (status_code >= 400) {
    status = gaus_create_error(__func__, GAUS_HTTP_ERROR, status_code,
                               "Posting authenticate failed with http error code %d",
                               status_code);
    goto error;
  }
  error:
  free(report_post_body);
  free(raw_report_result);
  free(query_parms);
  json_decref(json_to_send);
  return status;
}

// Produces {s:s, s:s, s:s, ...} for however many v_ints are passed in.  All keys need to be
// unique or they will be ignored. (We just add missing keys to the json object)
static gaus_error_t *
get_json_for_vints(unsigned int int_count, gaus_v_int_t *v_ints, json_t **json_v_ints) {
  gaus_error_t *status = NULL;
  *json_v_ints = json_object();

  for (unsigned int i = 0; i < int_count; i++) {
    json_t *this_v_float = json_pack("{s:i}", v_ints[i].name, v_ints[i].value);
    if (this_v_float == NULL) {
      status = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500, "Error parsing v_ints");
    }
    int result = json_object_update_missing(*json_v_ints, this_v_float);
    if (result != 0) {
      status = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500, "Error encoding v_ints");
    }
    json_decref(this_v_float);
  }
  return status;
}

// Produces {s:s, s:s, s:s, ...} for however many v_floats are passed in.  All keys need to be
// unique or they will be ignored. (We just add missing keys to the json object)
static gaus_error_t *
get_json_for_vfloats(unsigned int float_count, gaus_v_float_t *v_floats, json_t **json_v_floats) {
  gaus_error_t *status = NULL;
  *json_v_floats = json_object();

  for (unsigned int i = 0; i < float_count; i++) {
    json_t *this_v_float = json_pack("{s:f}", v_floats[i].name, v_floats[i].value);
    if (this_v_float == NULL) {
      status = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500, "Error parsing v_floats");
    }
    int result = json_object_update_missing(*json_v_floats, this_v_float);
    if (result != 0) {
      status = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500, "Error encoding v_floats");
    }
    json_decref(this_v_float);
  }
  return status;
}

// Produces {s:s, s:s, s:s, ...} for however many v_strings are passed in.  All keys need to be
// unique or they will be ignored. (We just add missing keys to the json object)
static gaus_error_t *
get_json_for_vstrings(unsigned int string_count, gaus_v_string_t *v_strings, json_t **json_v_strings) {
  gaus_error_t *status = NULL;
  *json_v_strings = json_object();

  for (unsigned int i = 0; i < string_count; i++) {
    json_t *this_v_string = json_pack("{s:s}", v_strings[i].name, v_strings[i].value);
    if (this_v_string == NULL) {
      status = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500, "Error parsing v_strings");
    }
    int result = json_object_update_missing(*json_v_strings, this_v_string);
    if (result != 0) {
      status = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500, "Error encoding v_strings");
    }
    json_decref(this_v_string);
  }
  return status;
}

static gaus_error_t *create_json_for_header(const gaus_report_header_t *header, json_t **json_header) {
  gaus_error_t *status = NULL;
  if (!(*json_header = json_pack("{s:s}", TS_JSON, header->ts))) {
    status = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500, "Error encoding header");
  }
  return status;
}

static gaus_error_t *
create_json_for_update_status(const gaus_report_t *report, json_t **json_report) {
  gaus_error_t *status = NULL;
  json_t *json_vstrings = NULL;

  if (report->report_type != GAUS_REPORT_UPDATE) {
    status = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500,
                               "Attempted to create an update from wrong report type");
    goto error;
  }

  *json_report = json_object();
  if (*json_report == NULL) {
    status = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500,
                               "Unable to create json object");
    goto error;
  }
  if (0 != json_object_set_new(*json_report, TYPE_JSON, json_string(UPDATE_STATUS_TYPE_JSON))) {
    status = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500, "Error encoding update status type");
    goto error;
  }

  if (0 != json_object_set_new(*json_report, TS_JSON, json_string(report->report.update_status.ts))) {
    status = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500, "Error encoding update status timestamp");
    goto error;
  }

  if (NULL != (status = get_json_for_vstrings(report->report.update_status.v_string_count,
                                              report->report.update_status.v_strings,
                                              &json_vstrings))) {
    goto error;
  }
  if (json_vstrings) {
    if (0 != json_object_set(*json_report, V_STRINGS_JSON, json_vstrings)) {
      status = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500, "Error encoding vstrings to report");
      goto error;
    }
  } else {
    logging(L_WARNING, "No json_vstrings in event.update.Status type report!");
  }

  error:
  json_decref(json_vstrings);
  return status;
}


static gaus_error_t *
create_json_for_generic(const gaus_report_t *report, json_t **json_report) {
  gaus_error_t *status = NULL;
  json_t *json_vints = NULL;
  json_t *json_vfloats = NULL;
  json_t *json_vstrings = NULL;
  char *temp_type = NULL;

  if (report->report_type != GAUS_REPORT_GENERIC) {
    status = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500,
                               "Attempted to create a generic from wrong report type");
    goto error;
  }

  *json_report = json_object();
  if (*json_report == NULL) {
    status = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500,
                               "Unable to create json object");
    goto error;
  }

  int required_size = snprintf(NULL, 0, "%s%s", UPDATE_GENERIC_TYPE_JSON, report->report.generic.type) + 1;
  temp_type = malloc(required_size);
  sprintf(temp_type, "%s%s", UPDATE_GENERIC_TYPE_JSON, report->report.generic.type);

  if (0 != json_object_set_new(*json_report, TYPE_JSON, json_string(temp_type))) {
    status = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500, "Error encoding update status type");
    goto error;
  }

  if (0 != json_object_set_new(*json_report, TS_JSON, json_string(report->report.update_status.ts))) {
    status = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500, "Error encoding update status timestamp");
    goto error;
  }

  //v_ints
  if (NULL != (status = get_json_for_vints(report->report.update_status.v_int_count,
                                           report->report.update_status.v_ints,
                                           &json_vints))) {
    goto error;
  }

  if (0 != json_object_set(*json_report, V_INTS_JSON, json_vints)) {
    status = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500, "Error encoding vints to report");
    goto error;
  }

  //v_floats
  if (NULL != (status = get_json_for_vfloats(report->report.update_status.v_float_count,
                                             report->report.update_status.v_floats,
                                             &json_vfloats))) {
    goto error;
  }

  if (0 != json_object_set(*json_report, V_FLOATS_JSON, json_vfloats)) {
    status = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500, "Error encoding vfloats to report");
    goto error;
  }

  //v_strings
  if (NULL != (status = get_json_for_vstrings(report->report.update_status.v_string_count,
                                              report->report.update_status.v_strings,
                                              &json_vstrings))) {
    goto error;
  }

  if (0 != json_object_set(*json_report, V_STRINGS_JSON, json_vstrings)) {
    status = gaus_create_error(__func__, GAUS_UNKNOWN_ERROR, 500, "Error encoding vstrings to report");
    goto error;
  }

  error:
  free(temp_type);
  json_decref(json_vints);
  json_decref(json_vfloats);
  json_decref(json_vstrings);
  return status;
}
