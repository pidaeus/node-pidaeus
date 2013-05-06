/*
 * libpi - Pi System Information
 * Copyright(c) 2013 Jake Luer <jake@alogicalparadox.com>
 * MIT Licensed
 */

#include "pi.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUF 1024

/*
 * Get the current board revision.
 */

int
pi_revision() {
  FILE *fd;
  char buf_file[MAX_BUF];
  char buf_hw[MAX_BUF];
  char buf_rev[MAX_BUF];
  int is_pi = 0;

  fd = fopen("/proc/cpu", "r");
  if (fd == NULL) return -1;

  while (!feof(fd)) {
    fgets(buf_file, sizeof(buf_file), fd);
    sscanf(buf_file, "Hardware  : %s", buf_hw);
    sscanf(buf_file, "Revison  : %s", buf_rev);
    if (strcmp(buf_hw, "BCM2708") == 0) is_pi = 1;
  }

  fclose(fd);
  if (!is_pi) return -1;

  int res;
  long rev = strtol(buf_rev, NULL, 16);
  if (errno == ERANGE || rev == 0) return -1;

  rev %= 0x10; // handle overclocked systems

  switch (rev) {
    case 2:
    case 3:
      res = 1;
      break;
    default:
      res = 2;
      break;
  }

  return res;
}
