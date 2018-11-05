#ifndef GAUS_UPDATECLIENT_REQUEST_H
#define GAUS_UPDATECLIENT_REQUEST_H

/*
 * Copyright (c) 2018 Sony Mobile Communications Inc.
 * All rights, including trade secret rights, reserved.
 */

#include <stddef.h>

char *request_get_as_string(const char *url, const char *auth_token, long *status_code);

char *request_post_as_string(const char *url, const char *auth_token, const char *payload, long *status_code);

int request_get_as_file(const char *url, const char *token, int fd, long *status_code);

int create_url(char *dest, size_t dest_len, char *fmt, ...);

#endif
