
#include "gpio.h"

extern "C" void
init (Handle<Object> target) {
  HandleScope scope;
  GPIO::Initialize(target);
}

NODE_MODULE(pidaeus, init)
