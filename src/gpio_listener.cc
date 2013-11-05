#include <stdio.h>
#include <string.h>
#include <sstream>

#include "gpio_listener.h"

#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define MAX_BUF 64

namespace pidaeus {




GpioListener::GpioListener(
    GpioListenerQueue *queue
  , GpioListenerOptions *options
) : queue(queue), options(options) {
  _handle = reinterpret_cast<uv_handle_t*>(&handle);
  _handle->data = this;
};

GpioListener::~GpioListener() {};

void
GpioListener::Start() {
  std::stringstream path;
  path << SYSFS_GPIO_DIR << "/gpio" << options->handle->pin << "/value";
  uv_fs_event_init(uv_default_loop(), &handle, path.str().c_str(), OnEvent, 0);
}

void
GpioListener::Stop() {
  uv_close(_handle, OnClose);
}

void
GpioListener::OnEvent(uv_fs_event_t *handle, const char *filename, int events, int status) {
  if (events & UV_CHANGE) {
    GpioListener* listener = static_cast<GpioListener*>(handle->data);
    pi_gpio_pin_t pin = listener->options->handle->pin;
    pi_gpio_value_t value = PI_GPIO_LOW;
    GpioListenerResult *res = new GpioListenerResult(pin, value);
    listener->queue->queue.push(res);
    uv_async_send(&listener->queue->pulse);
  }
}

void
GpioListener::OnClose(uv_handle_t* handle) {
  //GpioListener* listener = static_cast<GpioListener*>(handle->data);
  // TODO: add event to remove queue
}


}
