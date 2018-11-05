#ifndef GAUS_GAUS_H
#define GAUS_GAUS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <gaus/gaus_client_types.h>

typedef struct {
  char *serverUrl;
  bool globalInitalized;
  char *proxy;
} gaus_global_state_t;

extern gaus_global_state_t gaus_global_state;

gaus_error_t *
gaus_create_error(const char *func, gaus_error_type_t type, unsigned int code, const char *description, ...);


#ifdef __cplusplus
}
#endif
#endif //GAUS_GAUS_H
