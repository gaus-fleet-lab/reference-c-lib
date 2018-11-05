#ifndef GAUS_UPDATECLIENT_LOG_H
#define GAUS_UPDATECLIENT_LOG_H

/*
 * Copyright (c) 2018 Sony Mobile Communications Inc.
 * All rights, including trade secret rights, reserved.
 */

#define L_DEBUG 0
#define L_INFO 1
#define L_NOTICE 2
#define L_WARNING 3
#define L_ERROR 4

#define L_RAW (1 << 10) /* Modifier to log without timestamp */

void init_logging(void);

void logging(int level, const char *fmt, ...);

void set_loglevel(int level);

void set_loglevel_from_string(char *loglevel);

#endif
