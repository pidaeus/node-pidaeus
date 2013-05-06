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
 * Global memory address space
 */

static void *gpio_map_ctr;
volatile unsigned *gpio_map;

/*
 * Open the memory space for read/write access.
 */

int
pi_gpio_setup(void) {
  int fd;
  uint8_t *addr;

  fd = open("/dev/mem", O_RDWR | O_SYNC);
  if (fd < 0) return -1;

  addr = malloc(BLOCK_SIZE + (PAGE_SIZE - 1));
  if (addr == NULL) return -1;

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

  if (gpio_map_ctr == MAP_FAILED) return -1;
  gpio_map = (volatile unsigned *)gpio_map_ctr;
  debug("success");
  return 0;
}

/*
 * Unmap memory space.
 */

int
pi_gpio_teardown(void) {
  munmap(gpio_map_ctr, BLOCK_SIZE);
  debug("success");
  return 0;
}

/*
 * Claim usage of a pin with default options.
 */

pi_gpio_handle_t*
pi_gpio_claim(pi_gpio_pin_t gpio) {
  return pi_gpio_claim_with_args(gpio, PI_DIR_IN, PI_PULL_NONE);
}

/*
 * Claim usage of an input pin with pull options.
 */

pi_gpio_handle_t*
pi_gpio_claim_input(pi_gpio_pin_t gpio, pi_gpio_pull_t pull) {
  return pi_gpio_claim_with_args(gpio, PI_DIR_IN, pull);
}

/*
 * Claim usage of an output pin with default value.
 */

pi_gpio_handle_t*
pi_gpio_claim_output(pi_gpio_pin_t gpio, pi_gpio_value_t value) {
  pi_gpio_handle_t *handle = pi_gpio_claim_with_args(gpio, PI_DIR_OUT, PI_PULL_NONE);
  pi_gpio_write(handle, value);
  return handle;
}

/*
 * Claim usage of pin with custom options.
 */

pi_gpio_handle_t*
pi_gpio_claim_with_args(pi_gpio_pin_t gpio, pi_gpio_direction_t direction, pi_gpio_pull_t pull) {
  debug("(%i)", gpio);
  pi_gpio_handle_t *handle = malloc(sizeof(pi_gpio_handle_t));
  handle->gpio = gpio;
  handle->error = 0;
  pi_gpio_set_pull(handle, pull);
  pi_gpio_set_direction(handle, direction);
  return handle;
}

/*
 * Set direction of a claimed pin. Usually invoked automatically.
 */

void
pi_gpio_set_direction(pi_gpio_handle_t *handle, pi_gpio_direction_t direction) {
  int gpio = handle->gpio;
  int offset = FSEL_OFFSET + (gpio / 10);
  int shift = (gpio % 10) * 3;
  debug("(%i) %s", gpio, direction == PI_DIR_OUT ? "out": "in");
  *(gpio_map + offset) = (*(gpio_map + offset) & ~(7 << shift)) | (direction << shift);
}

/*
 * Get direction of a claimed pin.
 */

pi_gpio_direction_t
pi_gpio_get_direction(pi_gpio_handle_t *handle) {
  int gpio = handle->gpio;
  int offset = FSEL_OFFSET + (gpio / 10);
  int shift = (gpio % 10) * 3;
  int value = *(gpio_map + offset);
  value >>= shift;
  value &= 7;
  return value == 0 ? PI_DIR_IN : PI_DIR_OUT;
}

/*
 * Set software pull up/down value for input pins.
 */

void
pi_gpio_set_pull(pi_gpio_handle_t *handle, pi_gpio_pull_t pull) {
  int gpio = handle->gpio;
  int offset = PULLUPDNCLK_OFFSET + (gpio / 32);
  int shift = (gpio % 32);

  switch (pull) {
    case PI_PULL_DOWN:
    case PI_PULL_UP:
      debug("(%i) %s", gpio, pull == PI_PULL_UP ? "up" : "down");
      *(gpio_map + PULLUPDN_OFFSET) = (*(gpio_map + PULLUPDN_OFFSET) & ~3) | pull;
      break;
    default:
      debug("(%i) none", gpio);
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
  int gpio = handle->gpio;
  int offset = PINLEVEL_OFFSET + (gpio / 32);
  int mask = (1 << gpio % 32);
  int value = *(gpio_map + offset) & mask;
  return value == 0 ? PI_GPIO_LOW : PI_GPIO_HIGH;
}

/*
 * Write value to an output pin.
 */

int
pi_gpio_write(pi_gpio_handle_t *handle, pi_gpio_value_t value) {
  pi_gpio_pin_t gpio = handle->gpio;
  int shift = (gpio % 32);
  int offset;

  switch (value) {
    case PI_GPIO_HIGH:
      debug("(%i) PI_GPIO_HIGH", gpio);
      offset = SET_OFFSET + (gpio / 32);
      break;
    case PI_GPIO_LOW:
      debug("(%i) PI_GPIO_LOW", gpio);
      offset = CLR_OFFSET + (gpio / 32);
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
  debug("(%i)", handle->gpio);
  pi_gpio_direction_t direction = pi_gpio_get_direction(handle);

  if (direction == PI_DIR_OUT) {
    pi_gpio_set_direction(handle, PI_DIR_IN);
  }

  free(handle);
  return 0;
}

