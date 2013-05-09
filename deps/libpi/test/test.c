
#include "pi.h"

#include <assert.h>
#include <stdio.h>

#define PI_REVISION 2


#define RUN_TEST(func) \
  test_##func(); \
  fprintf(stdout, "  (y) test_" #func "()\n");

void
test_pi_revision(void) {
  int revision = pi_revision();
  assert(revision == PI_REVISION);
}

void
test_pi_closure_default(void) {
  pi_closure_t *closure = pi_default_closure();
  pi_closure_t *d_closure = pi_default_closure();
  assert(closure == d_closure);
  pi_closure_delete(d_closure);
}

void
test_pi_closure_custom(void) {
  pi_closure_t *closure = pi_closure_new();
  pi_closure_t *c_closure = pi_closure_new();
  assert(closure != c_closure);
  pi_closure_delete(closure);
  pi_closure_delete(c_closure);
}

int
main() {
  fprintf(stdout, "\n");
  RUN_TEST(pi_revision)
  RUN_TEST(pi_closure_default)
  RUN_TEST(pi_closure_custom)
  fprintf(stdout, "\n");
  return 0;
}
