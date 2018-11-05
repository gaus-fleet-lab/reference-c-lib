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
