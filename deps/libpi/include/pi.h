/*
 * libpi
 * Copyright(c) 2013 Jake Luer <jake@alogicalparadox.com>
 * MIT Licensed
 */

#ifndef PI_H
#define PI_H

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
  PI_DIR_IN  = 0x00,
  PI_DIR_OUT = 0x01
} pi_gpio_direction_t;

typedef enum {
  PI_EDGE_NONE    = 0x00,
  PI_EDGE_RISING  = 0x01,
  PI_EDGE_FALLING = 0x02,
  PI_EDGE_BOTH    = 0x03
} pi_gpio_edge_t;

typedef enum {
  PI_PULL_NONE  = 0x00,
  PI_PULL_DOWN  = 0x01,
  PI_PULL_UP    = 0x02
} pi_gpio_pull_t;

typedef struct {
  pi_gpio_pin_t gpio;
  int error;
} pi_gpio_handle_t;

/*
 * cpuinfo.c
 */

PI_EXTERN int
pi_revision(void);

/*
 * gpio_mmap.c
 */

PI_EXTERN int
pi_gpio_setup(void);

PI_EXTERN int
pi_gpio_teardown(void);

PI_EXTERN pi_gpio_handle_t*
pi_gpio_claim(pi_gpio_pin_t gpio);

PI_EXTERN pi_gpio_handle_t*
pi_gpio_claim_input(pi_gpio_pin_t gpio, pi_gpio_pull_t pull);

PI_EXTERN pi_gpio_handle_t*
pi_gpio_claim_output(pi_gpio_pin_t gpio, pi_gpio_value_t value);

PI_EXTERN pi_gpio_handle_t*
pi_gpio_claim_with_args(pi_gpio_pin_t gpio, pi_gpio_direction_t direction,
  pi_gpio_pull_t pull);

PI_EXTERN void
pi_gpio_set_direction(pi_gpio_handle_t *handle, pi_gpio_direction_t direction);

PI_EXTERN pi_gpio_direction_t
pi_gpio_get_direction(pi_gpio_handle_t *handle);

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
pi_gpio_listener_claim(pi_gpio_pin_t gpio);

PI_EXTERN int
pi_gpio_listen(pi_gpio_handle_t *listener, pi_gpio_edge_t edge);

PI_EXTERN void
pi_gpio_listener_release(pi_gpio_handle_t *listener);

/*
 * End
 */

#ifdef __cplusplus
}
#endif

#endif /* PI_H */
