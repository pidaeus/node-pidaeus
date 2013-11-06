#include "pi.h"

#include <time.h>

#define PIN_ENABLE 18
#define PIN_COIL_A1 4
#define PIN_COIL_A2 17
#define PIN_COIL_B1 23
#define PIN_COIL_B2 24

pi_closure_t *closure;
pi_gpio_handle_t *pin_enable;
pi_gpio_handle_t *pin_coil_a1;
pi_gpio_handle_t *pin_coil_a2;
pi_gpio_handle_t *pin_coil_b1;
pi_gpio_handle_t *pin_coil_b2;

void set_step(int w1, int w2, int w3, int w4) {
  pi_gpio_write(pin_coil_a1, w1);
  pi_gpio_write(pin_coil_a2, w2);
  pi_gpio_write(pin_coil_b1, w3);
  pi_gpio_write(pin_coil_b2, w4);
}

void move_forward(unsigned long ms, int steps) {
  struct timespec wait = {0};
  wait.tv_nsec = ms * 1000000L;

  int i;
  for (i = 0; i < steps; i++) {
    set_step(1, 0, 1, 0);
    nanosleep(&wait, NULL);
    set_step(0, 1, 1, 0);
    nanosleep(&wait, NULL);
    set_step(0, 1, 0, 1);
    nanosleep(&wait, NULL);
    set_step(1, 0, 0, 1);
    nanosleep(&wait, NULL);
  }
}

void move_backward(unsigned long ms, int steps) {
  struct timespec wait = {0};
  wait.tv_nsec = ms * 1000000L;

  int i;
  for (i = 0; i < steps; i++) {
    set_step(1, 0, 0, 1);
    nanosleep(&wait, NULL);
    set_step(0, 1, 0, 1);
    nanosleep(&wait, NULL);
    set_step(0, 1, 1, 0);
    nanosleep(&wait, NULL);
    set_step(1, 0, 1, 0);
    nanosleep(&wait, NULL);
  }
}

int main() {
  closure = pi_default_closure();
  pi_gpio_setup(closure);

  pi_gpio_pin_t gpioe = PIN_ENABLE;
  pi_gpio_pin_t gpioa1 = PIN_COIL_A1;
  pi_gpio_pin_t gpioa2 = PIN_COIL_A2;
  pi_gpio_pin_t gpiob1 = PIN_COIL_B1;
  pi_gpio_pin_t gpiob2 = PIN_COIL_B2;

  pin_enable = pi_gpio_claim_output(closure, gpioe, PI_GPIO_HIGH);
  pin_coil_a1 = pi_gpio_claim_output(closure, gpioa1, PI_GPIO_LOW);
  pin_coil_a2 = pi_gpio_claim_output(closure, gpioa2, PI_GPIO_LOW);
  pin_coil_b1 = pi_gpio_claim_output(closure, gpiob1, PI_GPIO_LOW);
  pin_coil_b2 = pi_gpio_claim_output(closure, gpiob2, PI_GPIO_LOW);

  move_forward(5, 356);
  move_backward(30, 45);

  pi_gpio_release(pin_enable);
  pi_gpio_release(pin_coil_a1);
  pi_gpio_release(pin_coil_a2);
  pi_gpio_release(pin_coil_b1);
  pi_gpio_release(pin_coil_b2);

  pi_gpio_teardown(closure);

  return 0;
}
