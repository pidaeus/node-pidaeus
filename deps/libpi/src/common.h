/*
 * libpi - Internal bootstrap
 * Copyright(c) 2013 Jake Luer <jake@alogicalparadox.com>
 * MIT Licensed
 */

#ifndef PI_COMMON_H
#define PI_COMMON_H

#include <stdio.h>

/*
 * Get current DEBUG variable.
 */

#ifdef DEBUG
#define PI_DEBUG 1
#else
#define PI_DEBUG 0
#endif

/*
 * Provide debug bootstrap.
 */

#define pi__debug_print(fmt, mod, args...)                                    \
  do {                                                                        \
    if (PI_DEBUG) {                                                           \
      fprintf(stderr                                                          \
      , " \033[94m%s\033[92m ln:%d \033[93m(%s)\033[90m " fmt "\033[0m\n"     \
      , mod, __LINE__, __FUNCTION__, ##args);                                 \
    }                                                                         \
  } while (0)

/*
 * timer.c
 */

void
pi_sleep_ms(unsigned long ms);

void
pi_sleep_ns(unsigned long ns);

/*
 * Stop
 */

#endif
