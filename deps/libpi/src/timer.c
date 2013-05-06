/*
 * libpi - Timer
 * Copyright(c) 2013 Jake Luer <jake@alogicalparadox.com>
 * MIT Licensed
 */

#include "pi.h"
#include "common.h"

#include <time.h>

/*
 * Sleep for miliseconds.
 */

void
pi_sleep_ms(unsigned long ms) {
  struct timespec wait = {0};
  wait.tv_nsec = ms * 1000000L;
  nanosleep(&wait, NULL);
}

/*
 * Sleep for nanoseconds.
 */

void
pi_sleep_ns(unsigned long ns) {
  struct timespec wait = {0};
  wait.tv_nsec = ns;
  nanosleep(&wait, NULL);
}
