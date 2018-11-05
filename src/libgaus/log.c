/*
 * Copyright (c) 2018 Sony Mobile Communications Inc.
 * All rights, including trade secret rights, reserved.
 */

#include "log.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <syslog.h>
#include <time.h>

#define LOG_MAX_LEN 1024 /* Default maximum length of syslog messages */

#ifdef DEBUG
#define VERBOSITY L_DEBUG
#else
#define VERBOSITY L_INFO
#endif

#ifndef SYSLOG_ENABLED
#define SYSLOG_ENABLED 0
#endif

static int verbosity_level = VERBOSITY;
static int log_to_stdout = 1;
static int syslog_enabled = SYSLOG_ENABLED;
static int systemd_output = 0;

static void log_raw(int level, const char *msg);

void init_logging(void) {
  char *uses_journald = getenv("JOURNAL_STREAM");
  if (uses_journald) {
    systemd_output = 1;
  }
}

void set_loglevel(int level) {
  verbosity_level = level;
}

static void log_raw(int level, const char *msg) {
  static const int syslog_level_map[] = {LOG_DEBUG, LOG_INFO, LOG_NOTICE,
                                         LOG_WARNING, LOG_ERR};
  static const int systemd_level_map[] = {7, 6, 5, 4, 3};
  char buf[64];
  int rawmode = (level & L_RAW);
  level &= 0xff; /* clear flags */
  if (level < verbosity_level) {
    return;
  }

  if (log_to_stdout) {
    if (rawmode) {
      fprintf(stdout, "%s\n", msg);
    } else if (systemd_output) {
      fprintf(stdout, "<%d>%s\n", systemd_level_map[level], msg);
    } else {
      int off;
      struct timeval tv;

      gettimeofday(&tv, NULL);
      off =
          strftime(buf, sizeof(buf), "%d %b %H:%M:%S.", localtime(&tv.tv_sec));
      snprintf(buf + off, sizeof(buf) - off, "%03d", (int) tv.tv_usec / 1000);
      fprintf(stdout, "%s - %s\n", buf, msg);
    }
    fflush(stdout);
  }
  if (syslog_enabled) {
    syslog(syslog_level_map[level], "%s", msg);
  }
}

void logging(int level, const char *fmt, ...) {
  va_list ap;
  char msg[LOG_MAX_LEN];

  if ((level & 0xff) < verbosity_level) {
    return;
  }

  va_start(ap, fmt);
  vsnprintf(msg, sizeof(msg), fmt, ap);
  va_end(ap);

  log_raw(level, msg);
}

void set_loglevel_from_string(char *loglevel) {
  if (!loglevel) {
    return;
  }
  if (strcmp(loglevel, "debug") == 0) {
    set_loglevel(L_DEBUG);
  } else if (strcmp(loglevel, "info") == 0) {
    set_loglevel(L_INFO);
  } else if (strcmp(loglevel, "notice") == 0) {
    set_loglevel(L_NOTICE);
  } else if (strcmp(loglevel, "warning") == 0) {
    set_loglevel(L_WARNING);
  } else if (strcmp(loglevel, "error") == 0) {
    set_loglevel(L_ERROR);
  }
}
