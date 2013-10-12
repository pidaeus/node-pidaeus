/*!
 * External includes
 */

#include <node.h>

/*!
 * Source controlled includes
 */

#include "nan.h"

/*!
 * Local includes
 */

#include "gpio.h"

/*!
 * Start namespace
 */

namespace pidaeus {

/**
 * Expose our constructor functions to user-land.
 */

void Init (v8::Handle<v8::Object> target) {
  GPIO::Init();

  v8::Local<v8::Function> gpioc = v8::FunctionTemplate::New(GPIOConstruct)->GetFunction();
  target->Set(NanSymbol("GPIO"), gpioc);
}

/*!
 * Register node module.
 */

NODE_MODULE(pidaeus, Init)

} // end namespace
