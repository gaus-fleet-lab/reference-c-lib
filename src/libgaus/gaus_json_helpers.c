//The MIT License (MIT)
//
//Copyright 2018, Sony Mobile Communications Inc.
//
//Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#include "gaus_json_helpers.h"
#include "log.h"

#include <jansson.h>
#include <string.h>

const char *get_dict_string(json_t *dict, char *key, char *default_value) {
  json_t *json_value = json_object_get(dict, key);
  if (json_value == NULL) {
    return default_value;
  }
  if (!json_is_string(json_value)) {
    logging(L_WARNING, "\"%s\" is not a string", key);
  }
  return json_string_value(json_value);
}

/* Read key from JSON object (dict).
   Returns default_value if key is not present.
 */
char *dup_dict_string(json_t *dict, char *key, char *default_value) {
  const char *string = get_dict_string(dict, key, default_value);
  if (!string) {
    return NULL;
  }
  return strdup(string);
}

/* Read key from JSON object (dict).
   Returns default_value if key is not present.
 */
int get_dict_int(json_t *dict, char *key, int default_value) {
  json_t *json_value = json_object_get(dict, key);
  if (json_value == NULL) {
    return default_value;
  }
  if (!json_is_integer(json_value)) {
    logging(L_WARNING, "\"%s\" is not an integer", key);
    return default_value;
  }
  return json_integer_value(json_value);
}
