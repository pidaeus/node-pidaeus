#include "pi.h"

#include <stdlib.h>
#include <signal.h>

#define GPIO_BUTTON 23
#define GPIO_LED 18

pi_closure_t *closure;
pi_gpio_handle_t *led;
pi_gpio_handle_t *button;
pi_gpio_handle_t *buttonmap;

void signal_handler(int sig) {
  if (sig == SIGINT) {
    pi_gpio_write(led, PI_GPIO_LOW); // turn off led
    pi_gpio_listener_release(button);
    pi_gpio_release(buttonmap);
    pi_gpio_release(led);
    pi_gpio_teardown(closure);
    exit(0);
  }
}

int main() {
  signal(SIGINT, signal_handler);
  closure = pi_default_closure();
  pi_gpio_setup(closure);

  int last = 0;
  pi_gpio_pin_t pin_button = GPIO_BUTTON;
  pi_gpio_pin_t pin_led = GPIO_LED;

  // set button to pullup
  buttonmap = pi_gpio_claim_input(closure, pin_button, PI_GPIO_PULL_UP);

  button = pi_gpio_listener_claim(pin_button, PI_GPIO_EDGE_BOTH);
  led = pi_gpio_claim_output(closure, pin_led, PI_GPIO_LOW);

  while (1) {
    pi_gpio_listen(button);
    last = last == 0 ? 1 : 0;
    pi_gpio_write(led, last == 1 ? PI_GPIO_HIGH : PI_GPIO_LOW);
    pi_gpio_listen(button);
  }

  return 0;
}
