//The MIT License (MIT)
//
//Copyright 2018, Sony Mobile Communications Inc.
//
//Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#ifndef GAUS_GAUS_JSON_DEFINES_H
#define GAUS_GAUS_JSON_DEFINES_H

#include <jansson.h>

//Register specific json defines:
#define DEVICE_ID_JSON "deviceId"
#define PRODUCT_AUTH_PARAM_JSON "productAuthParameters"
#define POLL_INTERVAL_JSON "pollIntervalSeconds"

//Authenticate specific json defines:
#define DEVICE_AUTH_PARAM_JSON "deviceAuthParameters"
#define TOKEN_JSON "token"

//Check for updates specific json defines:
#define METADATA_JSON "metadata"
#define UPDATES_JSON "updates"
#define SIZE_JSON "size"
#define UPDATE_TYPE_JSON "updateType"
#define PACKAGE_TYPE_JSON "packageType"
#define MD5_JSON "md5"
#define UPDATE_ID_JSON "updateId"
#define PACKAGE_TYPE_FILE_JSON "file"

//Report specific json defines:
#define TYPE_JSON "type"
#define UPDATE_GENERIC_TYPE_JSON "event.generic."
#define UPDATE_STATUS_TYPE_JSON "event.update.Status"
#define TS_JSON "ts"
#define V_INTS_JSON "v_ints"
#define V_FLOATS_JSON "v_floats"
#define V_STRINGS_JSON "v_strings"
#define VERSION_1_0_0_JSON "1.0.0"
#define HEADER_JSON "header"
#define DATA_JSON "data"

//Shared json defines:
#define DEVICE_AUTH_PARAM_JSON "deviceAuthParameters"
#define ACCESS_KEY_JSON "accessKey"
#define SECRET_KEY_JSON "secretKey"
#define DEVICE_GUID_JSON "deviceGUID"
#define PRODUCT_GUID_JSON "productGUID"
#define VERSION_JSON "version"
#define DOWNLOAD_URL_JSON "downloadUrl"

//Helper functions:
char *dup_dict_string(json_t *dict, char *key, char *default_value);

const char *get_dict_string(json_t *dict, char *key, char *default_value);

int get_dict_int(json_t *dict, char *key, int default_value);

#endif //GAUS_GAUS_JSON_DEFINES_H
