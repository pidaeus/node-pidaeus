/*
 * libpi
 * Copyright(c) 2013 Jake Luer <jake@alogicalparadox.com>
 * MIT Licensed
 */

#ifndef PI_H
#define PI_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Define visible export
 */

#if __GNUC__ >= 4
# define PI_EXTERN __attribute__((visibility("default")))
#else
# define PI_EXTERN /* noop */
#endif

/*
 * Closure type
 */

typedef struct {
  int revision;
  volatile uint32_t *gpio_map;
  volatile uint32_t *i2c_map;
} pi_closure_t;

/*
 * Type for GPIO pin
 */

typedef unsigned int pi_gpio_pin_t;

/*
 * Types for GPIO
 */

typedef enum {
  PI_GPIO_LOW  = 0x00,
  PI_GPIO_HIGH = 0x01
} pi_gpio_value_t;

typedef enum {
  PI_GPIO_MODE_INPUT  = 0x00,
  PI_GPIO_MODE_OUTPUT = 0x01
} pi_gpio_mode_t;

typedef enum {
  PI_GPIO_EDGE_NONE    = 0x00,
  PI_GPIO_EDGE_RISING  = 0x01,
  PI_GPIO_EDGE_FALLING = 0x02,
  PI_GPIO_EDGE_BOTH    = 0x03
} pi_gpio_edge_t;

typedef enum {
  PI_GPIO_PULL_NONE  = 0x00,
  PI_GPIO_PULL_DOWN  = 0x01,
  PI_GPIO_PULL_UP    = 0x02
} pi_gpio_pull_t;

typedef enum {
  PI_GPIO_METHOD_MMAP,
  PI_GPIO_METHOD_SYSFS
} pi_gpio_method_t;

typedef struct {
  pi_closure_t *closure;
  pi_gpio_pin_t pin;
  pi_gpio_method_t method;
  int error;
} pi_gpio_handle_t;

/*
 * closure.c
 */

PI_EXTERN pi_closure_t*
pi_default_closure(void);

PI_EXTERN pi_closure_t*
pi_closure_new(void);

PI_EXTERN void
pi_closure_delete(pi_closure_t *closure);

/*
 * cpuinfo.c
 */

PI_EXTERN int
pi_revision(void);

/*
 * gpio_mmap.c
 */

PI_EXTERN int
pi_gpio_setup(pi_closure_t *closure);

PI_EXTERN int
pi_gpio_teardown(pi_closure_t *closure);

PI_EXTERN pi_gpio_handle_t*
pi_gpio_claim(pi_closure_t *closure, pi_gpio_pin_t pin);

PI_EXTERN pi_gpio_handle_t*
pi_gpio_claim_input(pi_closure_t *closure, pi_gpio_pin_t pin,
  pi_gpio_pull_t pull);

PI_EXTERN pi_gpio_handle_t*
pi_gpio_claim_output(pi_closure_t *closure, pi_gpio_pin_t pin,
  pi_gpio_value_t value);

PI_EXTERN pi_gpio_handle_t*
pi_gpio_claim_with_args(pi_closure_t *closure, pi_gpio_pin_t pin,
  pi_gpio_mode_t mode, pi_gpio_pull_t pull);

PI_EXTERN void
pi_gpio_set_mode(pi_gpio_handle_t *handle, pi_gpio_mode_t mode);

PI_EXTERN pi_gpio_mode_t
pi_gpio_get_mode(pi_gpio_handle_t *handle);

PI_EXTERN void
pi_gpio_set_pull(pi_gpio_handle_t *handle, pi_gpio_pull_t pull);

PI_EXTERN pi_gpio_value_t
pi_gpio_read(pi_gpio_handle_t *handle);

PI_EXTERN int
pi_gpio_write(pi_gpio_handle_t *handle, pi_gpio_value_t value);

PI_EXTERN int
pi_gpio_release(pi_gpio_handle_t* handle);

/*
 * gpio_events.c
 */

PI_EXTERN pi_gpio_handle_t*
pi_gpio_listener_claim(pi_gpio_pin_t pin, pi_gpio_edge_t edge);

PI_EXTERN int
pi_gpio_listen(pi_gpio_handle_t *listener);

PI_EXTERN void
pi_gpio_listener_release(pi_gpio_handle_t *listener);

/*
 * End
 */

#ifdef __cplusplus
}
#endif

#endif /* PI_H */
