#include <math.h>
#include <time.h>
#include <stdio.h>

#include "pi.h"

struct timespec diff(struct timespec start, struct timespec end) {
  struct timespec temp;

  if ((end.tv_nsec - start.tv_nsec) < 0) {
    temp.tv_sec = end.tv_sec - start.tv_sec-1;
    temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
  } else {
    temp.tv_sec = end.tv_sec - start.tv_sec;
    temp.tv_nsec = end.tv_nsec - start.tv_nsec;
  }

  return temp;
}

int main() {
  pi_gpio_setup();

  struct timespec start, end;
  pi_gpio_pin_t led = 18;
  pi_gpio_handle_t *handle = pi_gpio_claim_output(led, PI_GPIO_LOW);
  int i;

  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

  for (i = 0; i < 500000; i++) {
    pi_gpio_write(handle, PI_GPIO_HIGH);
    pi_gpio_write(handle, PI_GPIO_LOW);
  }

  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

  pi_gpio_release(handle);
  pi_gpio_teardown();

  struct timespec timediff = diff(start, end);
  double seconds = ((timediff.tv_sec * 1e9) + timediff.tv_nsec) / 1e9;
  double herz = floor(i / seconds);

  printf("cycles: %d\n", i);
  printf("elapsed: %e\n", seconds);
  printf("frequency: %e KHz\n", herz / 1000);

  return 0;
}
