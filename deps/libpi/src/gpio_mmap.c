/*
 * libpi - GPIO interface
 * Copyright(c) 2013 Jake Luer <jake@alogicalparadox.com>
 * MIT Licensed
 */

#include "pi.h"
#include "common.h"

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

/*
 * Debug macro
 */

#define debug(fmt, args...) \
  pi__debug_print(fmt, "gpio_mmap", ##args)

/*
 * Gpio memory space
 */

#define BCM2708_PERI_BASE   0x20000000
#define GPIO_BASE           (BCM2708_PERI_BASE   +   0x200000)

#define FSEL_OFFSET          0
#define SET_OFFSET           7
#define CLR_OFFSET           10
#define PINLEVEL_OFFSET      13
#define PULLUPDN_OFFSET      37
#define PULLUPDNCLK_OFFSET   38

#define PAGE_SIZE    (4*1024)
#define BLOCK_SIZE   (4*1024)

/*
 * Open the memory space for read/write access.
 */

int
pi_gpio_setup(pi_closure_t *closure) {
  if (closure->gpio_map != NULL) {
    debug("already setup");
    return 0;
  }

  int fd;
  uint8_t *addr;
  uint32_t *gpio_map_ctr;

  fd = open("/dev/mem", O_RDWR | O_SYNC);
  if (fd < 0) {
    debug("error: cannot open /dev/mem");
    return -1;
  }

  addr = malloc(BLOCK_SIZE + (PAGE_SIZE - 1));
  if (addr == NULL) {
    debug("error: cannot malloc address");
    return -1;
  }

  if ((uint32_t)addr % PAGE_SIZE) {
    addr += PAGE_SIZE - ((uint32_t)addr % PAGE_SIZE);
  }

  gpio_map_ctr = mmap(
    addr,
    BLOCK_SIZE,
    PROT_READ | PROT_WRITE,
    MAP_SHARED | MAP_FIXED,
    fd,
    GPIO_BASE
  );

  if (gpio_map_ctr == MAP_FAILED) {
    debug("error: mmap failed");
    return -1;
  }

  closure->gpio_map = (volatile uint32_t*)gpio_map_ctr;
  debug("success");
  return 0;
}

/*
 * Unmap memory space.
 */

int
pi_gpio_teardown(pi_closure_t *closure) {
  munmap((uint32_t*)closure->gpio_map, BLOCK_SIZE);
  closure->gpio_map = NULL;
  debug("success");
  return 0;
}

/*
 * Claim usage of a pin with default options.
 */

pi_gpio_handle_t*
pi_gpio_claim(pi_closure_t *closure, pi_gpio_pin_t pin) {
  return pi_gpio_claim_with_args(closure, pin, PI_GPIO_MODE_INPUT, PI_GPIO_PULL_NONE);
}

/*
 * Claim usage of an input pin with pull options.
 */

pi_gpio_handle_t*
pi_gpio_claim_input(pi_closure_t *closure, pi_gpio_pin_t pin, pi_gpio_pull_t pull) {
  return pi_gpio_claim_with_args(closure, pin, PI_GPIO_MODE_INPUT, pull);
}

/*
 * Claim usage of an output pin with default value.
 */

pi_gpio_handle_t*
pi_gpio_claim_output(pi_closure_t *closure, pi_gpio_pin_t pin, pi_gpio_value_t value) {
  pi_gpio_handle_t *handle = pi_gpio_claim_with_args(closure, pin, PI_GPIO_MODE_OUTPUT, PI_GPIO_PULL_NONE);
  pi_gpio_write(handle, value);
  return handle;
}

/*
 * Claim usage of pin with custom options.
 */

pi_gpio_handle_t*
pi_gpio_claim_with_args(pi_closure_t *closure, pi_gpio_pin_t pin, pi_gpio_mode_t mode, pi_gpio_pull_t pull) {
  debug("(%i)", pin);
  pi_gpio_handle_t *handle = malloc(sizeof(pi_gpio_handle_t));
  handle->closure = closure;
  handle->method = PI_GPIO_METHOD_MMAP;
  handle->pin = pin;
  handle->error = 0;
  pi_gpio_set_pull(handle, pull);
  pi_gpio_set_mode(handle, mode);
  return handle;
}

/*
 * Set mode of a claimed pin. Usually invoked automatically.
 */

void
pi_gpio_set_mode(pi_gpio_handle_t *handle, pi_gpio_mode_t mode) {
  volatile uint32_t *gpio_map = handle->closure->gpio_map;
  int pin = handle->pin;
  int offset = FSEL_OFFSET + (pin / 10);
  int shift = (pin % 10) * 3;
  debug("(%i) %s", pin, mode == PI_GPIO_MODE_OUTPUT ? "out": "in");
  *(gpio_map + offset) = (*(gpio_map + offset) & ~(7 << shift)) | (mode << shift);
}

/*
 * Get mode of a claimed pin.
 */

pi_gpio_mode_t
pi_gpio_get_mode(pi_gpio_handle_t *handle) {
  volatile uint32_t *gpio_map = handle->closure->gpio_map;
  int pin = handle->pin;
  int offset = FSEL_OFFSET + (pin / 10);
  int shift = (pin % 10) * 3;
  int value = *(gpio_map + offset);
  value >>= shift;
  value &= 7;
  return value == 0 ? PI_GPIO_MODE_INPUT : PI_GPIO_MODE_OUTPUT;
}

/*
 * Set software pull up/down value for input pins.
 */

void
pi_gpio_set_pull(pi_gpio_handle_t *handle, pi_gpio_pull_t pull) {
  volatile uint32_t *gpio_map = handle->closure->gpio_map;
  int pin = handle->pin;
  int offset = PULLUPDNCLK_OFFSET + (pin / 32);
  int shift = (pin % 32);

  switch (pull) {
    case PI_GPIO_PULL_DOWN:
    case PI_GPIO_PULL_UP:
      debug("(%i) %s", pin, pull == PI_GPIO_PULL_UP ? "up" : "down");
      *(gpio_map + PULLUPDN_OFFSET) = (*(gpio_map + PULLUPDN_OFFSET) & ~3) | pull;
      break;
    default:
      debug("(%i) none", pin);
      *(gpio_map + PULLUPDN_OFFSET) &= ~3;
      break;
  }

  pi_sleep_ns(150);
  *(gpio_map + offset) = 1 << shift;
  pi_sleep_ns(150);
  *(gpio_map + PULLUPDN_OFFSET) &= ~3;
  *(gpio_map + offset) = 0;
}

/*
 * Read value of input pin.
 */

pi_gpio_value_t
pi_gpio_read(pi_gpio_handle_t *handle) {
  volatile uint32_t *gpio_map = handle->closure->gpio_map;
  int pin = handle->pin;
  int offset = PINLEVEL_OFFSET + (pin / 32);
  int mask = (1 << pin % 32);
  int value = *(gpio_map + offset) & mask;
  return value == 0 ? PI_GPIO_LOW : PI_GPIO_HIGH;
}

/*
 * Write value to an output pin.
 */

int
pi_gpio_write(pi_gpio_handle_t *handle, pi_gpio_value_t value) {
  volatile uint32_t *gpio_map = handle->closure->gpio_map;
  int pin = handle->pin;
  int shift = (pin % 32);
  int offset;

  switch (value) {
    case PI_GPIO_HIGH:
      debug("(%i) PI_GPIO_HIGH", pin);
      offset = SET_OFFSET + (pin / 32);
      break;
    case PI_GPIO_LOW:
    default:
      debug("(%i) PI_GPIO_LOW", pin);
      offset = CLR_OFFSET + (pin / 32);
      break;
  }

  *(gpio_map + offset) = 1 << shift;
  return 0;
}

/*
 * Release usage of a pin and free handle memory used.
 */

int
pi_gpio_release(pi_gpio_handle_t *handle) {
  debug("(%i)", handle->pin);
  pi_gpio_mode_t mode = pi_gpio_get_mode(handle);

  if (mode == PI_GPIO_MODE_OUTPUT) {
    pi_gpio_set_mode(handle, PI_GPIO_MODE_INPUT);
  }

  // jik ?
  handle->closure = NULL;
  free(handle);
  return 0;
}

