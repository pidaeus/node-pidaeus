/*!
 * Only once
 */

#ifndef __NODE_GPIO_LISTENER_H__
#define __NODE_GPIO_LISTENER_H__

#include <queue>
#include <uv.h>

#include <pi.h>

namespace pidaeus {

struct GpioListenerOptions {
  pi_gpio_handle_t *handle;
  pi_gpio_edge_t edge;
};

struct GpioListenerResult {
  GpioListenerResult(
      pi_gpio_pin_t pin
    , pi_gpio_value_t value
  ) : pin(pin), value(value)
  {}

  pi_gpio_pin_t pin;
  pi_gpio_value_t value;
};

struct GpioListenerQueue {
  std::queue<GpioListenerResult*> queue;
  uv_async_t pulse;
};

class GpioListener {
  public:
    GpioListener(GpioListenerQueue *queue, GpioListenerOptions *options);
    ~GpioListener();

    void Start();
    void Stop();

  private:
    static void OnEvent(uv_fs_event_t *handle, const char *filename, int events, int status);
    static void OnClose(uv_handle_t* handle);

    GpioListenerQueue *queue;
    GpioListenerOptions *options;

    uv_fs_event_t handle;
    uv_handle_t *_handle;

};

}

#endif
