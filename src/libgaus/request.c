//The MIT License (MIT)
//
//Copyright 2018, Sony Mobile Communications Inc.
//
//Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#include "log.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "curl_wrapper.h"
#include "gaus.h"
#include "gaus/gaus_client.h"


typedef struct FileResponse {
  int fd;
  FILE *file;
} FileResponse;

typedef struct InMemoryResponse {
  char *data;
  size_t pos;
} InMemoryResponse;

static int request_get(const char *url, const char *auth_token,
                       curl_write_callback response_writer, void *response, long *status_code);

static int request_post(const char *url, const char *auth_token, const char *payload,
                        curl_write_callback response_writer, void *response, long *status_code);

static size_t in_memory_response_writer(char *content, size_t size,
                                        size_t nmemb, void *userp);

static size_t file_response_writer(char *content, size_t size, size_t nmemb,
                                   void *userp);

static inline void write_char_safe(char *base, size_t *offset, size_t len, char ch) {
  if (base != NULL && *offset + 1 < len) {
    base[*offset] = ch;
  }
  (*offset)++;
}

/* snprintf-like function for constructing urls
 * Only accepts two format string directives:
 *   %s - string as given
 *   %e - urlencode the given string
 */
int create_url(char *dest, size_t dest_len, char *fmt, ...) {
  va_list ap;
  char *str;
  char *encoded;
  bool in_specifier = false;
  size_t pos = 0;

  CURL *curl = gaus_curl_easy_init();
  if (!curl) {
    logging(L_ERROR, "create_url: Failed to initialize curl");
    return -1;
  }

  va_start(ap, fmt);

  while (*fmt) {
    if (!in_specifier) {
      if (*fmt == '%') {
        in_specifier = true;
      } else {
        write_char_safe(dest, &pos, dest_len, *fmt);
      }
    } else {
      switch (*fmt) {
        case '%':
          write_char_safe(dest, &pos, dest_len, '%');
          break;
        case 'e':
          str = va_arg(ap, char *);
          encoded = curl_easy_escape(curl, str, strlen(str));
          str = encoded;
          while (*str) {
            write_char_safe(dest, &pos, dest_len, *str++);
          }
          curl_free(encoded);
          break;
        case 's':
          str = va_arg(ap, char *);
          while (*str) {
            write_char_safe(dest, &pos, dest_len, *str++);
          }
          break;
        default:
          logging(L_ERROR, "Invalid format string");
          goto out;
      }
      in_specifier = false;
    }
    fmt++;
  }
  out:
  gaus_curl_easy_cleanup(curl);
  if (dest) {
    if (pos + 1 < dest_len) {
      dest[pos] = '\0';
    } else {
      dest[dest_len - 1] = '\0';
      logging(L_ERROR, "create_url: URL too large, truncating (%s)", dest);
      return -1;
    }
  }
  va_end(ap);

  return pos;
}

int request_get_as_file(const char *url, const char *token, int fd, long *status_code) {
  FILE *file = fdopen(fd, "w");
  if (!file) {
    logging(L_ERROR, "Failed to open file");
    return 1;
  }
  FileResponse response = {.file = file, .fd = fd};

  int result = request_get(url, token, file_response_writer, &response, status_code);
  fclose(file);
  return result;
}

/* Returns the downloaded data as a string */
char *request_get_as_string(const char *url, const char *auth_token, long *status_code) {
  struct InMemoryResponse response = {};
  int err = request_get(url, auth_token, in_memory_response_writer, &response, status_code);
  if (err) {
    if (response.pos > 0) {
      logging(L_ERROR, "%s", response.data);
    }
    if (response.data) {
      free(response.data);
    }
    return NULL;
  }
  return response.data;
}

char *request_post_as_string(const char *url, const char *auth_token, const char *payload, long *status_code) {
  struct InMemoryResponse response = {};
  int err = request_post(url, auth_token, payload, in_memory_response_writer, &response, status_code);
  if (err) {
    if (response.pos > 0) {
      logging(L_ERROR, "%s", response.data);
    }
    if (response.data) {
      free(response.data);
    }
    return NULL;
  }
  return response.data;
}

static int request_post(const char *url, const char *auth_token, const char *payload,
                        curl_write_callback response_writer, void *response, long *status_code) {
  CURL *curl = NULL;
  CURLcode status;
  struct curl_slist *headers = NULL;
  long code;
  char *auth_header = NULL;
  char *user_agent_header = NULL;

  curl = gaus_curl_easy_init();
  if (!curl) {
    goto error;
  }

  if (auth_token) {
    size_t required_auth_header_len = snprintf(NULL, 0, "Authorization: Bearer %s", auth_token) + 1;
    auth_header = malloc(required_auth_header_len);
    size_t auth_header_len = snprintf(auth_header, required_auth_header_len,
                                      "Authorization: Bearer %s", auth_token);
    if (auth_header_len >= required_auth_header_len) {
      logging(L_ERROR, "request_get error: Authorization header to large");
      goto error;
    }
    headers = curl_slist_append(headers, auth_header);
  }

  gaus_version_t version = gaus_client_library_version();
  size_t required_user_agent_len =
      snprintf(NULL, 0, "User-Agent: gaus-device-client-c/v%d.%d.%d", version.major, version.minor, version.patch) + 1;
  user_agent_header = malloc(required_user_agent_len);
  size_t user_agent_len = snprintf(user_agent_header, required_user_agent_len,
                                   "User-Agent: gaus-device-client-c/v%d.%d.%d", version.major, version.minor,
                                   version.patch);
  if (user_agent_len >= required_user_agent_len) {
    logging(L_ERROR, "request_get error: User-Agent header too large");
    goto error;
  }
  headers = curl_slist_append(headers, user_agent_header);
  headers = curl_slist_append(headers, "Content-Type: application/json");

  if (gaus_global_state.proxy) {
    gaus_curl_easy_setopt(curl, CURLOPT_PROXY, gaus_global_state.proxy);
  }

  if (gaus_global_state.ca_path) {
    gaus_curl_easy_setopt(curl, CURLOPT_CAPATH, gaus_global_state.ca_path);
  }

  gaus_curl_easy_setopt(curl, CURLOPT_URL, url);
  gaus_curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload);
  gaus_curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(payload));
  gaus_curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

#ifdef GAUS_NO_CA_CHECK
  logging(L_DEBUG, "skipping verify peer certificate");
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif

  gaus_curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, response_writer);
  gaus_curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

  logging(L_DEBUG, "POST %s", url);
  status = gaus_curl_easy_perform(curl);
  if (status != 0) {
    logging(L_ERROR,
            "request_post error: unable to request data from %s:", url);
    logging(L_ERROR, "%s", curl_easy_strerror(status));
    goto error;
  }

  gaus_curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
  *status_code = code;
  if (code != 200) {
    logging(L_ERROR, "request_post error: server responded with code %ld for url: %s",
            code, url);
    logging(L_ERROR, "Failed post with payload: '%s'", payload);
    goto error;
  }

  free(auth_header);
  free(user_agent_header);
  gaus_curl_easy_cleanup(curl);
  curl_slist_free_all(headers);

  return 0;

  error:
  free(auth_header);
  free(user_agent_header);
  if (curl) {
    gaus_curl_easy_cleanup(curl);
  }
  if (headers) {
    curl_slist_free_all(headers);
  }
  return 1;
}

static int request_get(const char *url, const char *auth_token,
                       curl_write_callback response_writer, void *response, long *status_code) {
  CURL *curl = NULL;
  CURLcode status;
  struct curl_slist *headers = NULL;
  char *auth_header = NULL;
  char *user_agent_header = NULL;

  curl = gaus_curl_easy_init();
  if (!curl) {
    goto error;
  }

#ifdef GAUS_NO_CA_CHECK
  logging(L_DEBUG, "skipping verify peer certificate");
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif

  gaus_curl_easy_setopt(curl, CURLOPT_URL, url);
  gaus_curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);

  if (auth_token) {
    size_t required_auth_header_len = snprintf(NULL, 0, "Authorization: Bearer %s", auth_token) + 1;
    auth_header = malloc(required_auth_header_len);
    size_t auth_header_len = snprintf(auth_header, required_auth_header_len,
                                      "Authorization: Bearer %s", auth_token);
    if (auth_header_len >= required_auth_header_len) {
      logging(L_ERROR, "request_get error: Authorization header to large");
      goto error;
    }
    headers = curl_slist_append(headers, auth_header);
  }

  gaus_version_t version = gaus_client_library_version();
  size_t required_user_agent_len =
      snprintf(NULL, 0, "User-Agent: gaus-device-client-c/v%d.%d.%d", version.major, version.minor, version.patch) + 1;
  user_agent_header = malloc(required_user_agent_len);
  size_t user_agent_len = snprintf(user_agent_header, required_user_agent_len,
                                   "User-Agent: gaus-device-client-c/v%d.%d.%d", version.major, version.minor,
                                   version.patch);
  if (user_agent_len >= required_user_agent_len) {
    logging(L_ERROR, "request_get error: User-Agent header too large");
    goto error;
  }
  headers = curl_slist_append(headers, user_agent_header);
  gaus_curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

  if (gaus_global_state.proxy) {
    gaus_curl_easy_setopt(curl, CURLOPT_PROXY, gaus_global_state.proxy);
  }

  if (gaus_global_state.ca_path) {
    gaus_curl_easy_setopt(curl, CURLOPT_CAPATH, gaus_global_state.ca_path);
  }

  gaus_curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, response_writer);
  gaus_curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

  logging(L_DEBUG, "GET %s", url);
  status = gaus_curl_easy_perform(curl);
  if (status != 0) {
    logging(L_ERROR, "request_get error: %s", curl_easy_strerror(status));
    goto error;
  }

  gaus_curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, status_code);
  if (*status_code != 200) {
    logging(L_ERROR, "request_get error: server responded with code %ld", *status_code);
    goto error;
  }

  free(auth_header);
  free(user_agent_header);
  gaus_curl_easy_cleanup(curl);
  curl_slist_free_all(headers);

  return 0;

  error:
  free(auth_header);
  free(user_agent_header);
  if (curl) {
    gaus_curl_easy_cleanup(curl);
  }
  if (headers) {
    curl_slist_free_all(headers);
  }
  return -1;
}

static size_t in_memory_response_writer(char *content, size_t size, size_t nmemb, void *userp) {
  InMemoryResponse *resp = userp;
  size_t write_size = size * nmemb;
  size_t total_size = resp->pos + write_size + 1;
  /* If resp->data is NULL, then  the  call  is  equivalent  to  malloc(size) */
  resp->data = realloc(resp->data, total_size);

  if (resp->data == NULL) {
    logging(L_ERROR, "not enough memory (realloc returned NULL)\n");
    return 0;
  }

  memcpy(&(resp->data[resp->pos]), content, write_size);
  resp->pos += write_size;
  resp->data[resp->pos] = '\0'; /* Null terminate */

  logging(L_DEBUG, "Wrote %d bytes (+ NUL byte) to response (%d total)",
          write_size, resp->pos);
  if (resp->pos < 1000) {
    logging(L_DEBUG | L_RAW,
            "----[ data follows ]----\n%s\n"
            "------------------------",
            resp->data);
  }
  return write_size;
}

static size_t file_response_writer(char *content, size_t size, size_t nmemb, void *userp) {
  FileResponse *resp = userp;
  size_t write_size = size * nmemb;
  logging(L_DEBUG, "content size: %zu", write_size);
  if (resp->file == NULL) {
    logging(L_ERROR, "Cannot write to non-existing file handle");
    return 0;
  }

  size_t written = fwrite(content, size, nmemb, resp->file) * size;
  if (write_size != written) {
    logging(L_ERROR, "Failed to write to file");
    goto out;
  }
  logging(L_DEBUG, "Wrote %d bytes to file", write_size);
  out:
  return written;
}
