

#include "pi.h"
#include "common.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

/*
 * Debug macro
 */

#define debug(fmt, args...) \
  pi__debug_print(fmt, "i2c", ##args)

int
pi_i2c_setup(pi_closure_t *closure) {
  int fd;
  uint8_t;

  fd = open("/dev/mem", O_RDWR | O_SYNC);
  if (fd < 0) return -1;


}
