/*
 * libpi - Closure interface
 * Copyright(c) 2013 Jake Luer <jake@alogicalparadox.com>
 * MIT Licensed
 */

#include "pi.h"
#include "common.h"

#include <stdlib.h>
#include <string.h>

/*
 * Debug macro
 */

#define debug(fmt, args...) \
  pi__debug_print(fmt, "closure", ##args)

static pi_closure_t default_closure_struct;
static pi_closure_t* default_closure_inst;

int
pi__closure_init(pi_closure_t *closure) {
  memset(closure, 0, sizeof(*closure));
  closure->revision = pi_revision();
  closure->gpio_map = NULL;
  closure->i2c_map = NULL;
  return 0;
}

pi_closure_t*
pi_default_closure(void) {
  if (default_closure_inst) return default_closure_inst;
  debug("init default");
  pi__closure_init(&default_closure_struct);
  return (default_closure_inst = &default_closure_struct);
}

pi_closure_t*
pi_closure_new(void) {
  pi_closure_t *closure = malloc(sizeof(closure));
  if (closure == NULL) return NULL;
  pi__closure_init(closure);
  return closure;
}

void
pi_closure_delete(pi_closure_t *closure) {
  // TODO: teardowns
  if (closure == default_closure_inst) {
    debug("default");
    default_closure_inst = NULL;
  } else {
    debug("custom");
    free(closure);
  }
}
