#include "pi.h"

#include <stdlib.h>
#include <signal.h>

#ifndef GPIO_BUTTON
#define GPIO_BUTTON 23
#endif

#ifndef GPIO_LED
#define GPIO_LED 18
#endif

pi_gpio_handle_t *led;
pi_gpio_handle_t *button;

void signal_handler(int sig) {
  if (sig == SIGINT) {
    pi_gpio_write(led, PI_GPIO_LOW); // turn off led
    pi_gpio_listener_release(button);
    pi_gpio_release(led);
    pi_gpio_teardown();
    exit(0);
  }
}

int main() {
  signal(SIGINT, signal_handler);
  pi_gpio_setup();

  int last = 0;
  pi_gpio_pin_t gpio18 = 18;
  pi_gpio_pin_t gpio23 = 23;

  button = pi_gpio_listener_claim(gpio23);
  led = pi_gpio_claim_output(gpio18, PI_GPIO_LOW);

  while (1) {
    pi_gpio_listen(button, PI_EDGE_BOTH);
    last = last == 0 ? 1 : 0;
    pi_gpio_write(led, last == 1 ? PI_GPIO_HIGH : PI_GPIO_LOW);
    //pi_gpio_listen(button, PI_EDGE_BOTH);
  }

  return 0;
}
