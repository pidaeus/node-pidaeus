/*
 * libpi - Pi System Information
 * Copyright(c) 2013 Jake Luer <jake@alogicalparadox.com>
 * MIT Licensed
 */

#include "pi.h"
#include "common.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Debug macro
 */

#define debug(fmt, args...) \
  pi__debug_print(fmt, "cpuinfo", ##args)


#define MAX_BUF 1024

/*
 * Get the current board revision.
 */

int
pi_revision() {
  debug("start");
  FILE *fd;
  char buf_file[MAX_BUF];
  char buf_key[MAX_BUF];
  char buf_val[MAX_BUF];

  fd = fopen("/proc/cpuinfo", "r");
  if (fd == NULL) {
    debug("error: cannot open /proc/cpuinfo");
    return -1;
  }

  while (fgets(buf_file, sizeof(buf_file), fd) != NULL) {
    if (strlen(buf_file) == 1) continue;
    sscanf(buf_file, "%[^\t:] : %[^\t\n]", buf_key, buf_val);
    if (strncmp("Revision", buf_key, 8) == 0) break;
  }

  fclose(fd);

  long rev = strtol(buf_val, NULL, 16);
  if (errno == ERANGE || rev == 0) {
    debug("error: revision not found");
    return -1;
  }

  rev %= 0x10; // handle overclocked systems

  int res;
  switch (rev) {
    case 2:
    case 3:
      res = 1;
      break;
    default:
      res = 2;
      break;
  }

  debug("revision: %i", res);
  return res;
}
