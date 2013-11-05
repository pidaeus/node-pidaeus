/*!
 * Only once
 */

#ifndef __NODE_GPIO_H__
#define __NODE_GPIO_H__

/*!
 * External includes
 */

#include <node.h>
#include <string>
#include <queue>
#include <uv.h>
#include <v8.h>

/*!
 * Source controlled includes
 */

#include <pi.h>
#include "nan.h"


#include "gpio_listener.h"

/*!
 * Max pins
 */

#define PI_MAX_PINS 31

/*!
 * Setup ensures callback for async methods and constructs
 * options object.
 */

#define PI_GPIO_SETUP_COMMON(name, optionPos, callbackPos)                    \
  if (args.Length() == 0)                                                     \
    return NanThrowError(#name "() requires a callback argument");            \
  pidaeus::GPIO* gpio = node::ObjectWrap::Unwrap<pidaeus::GPIO>(args.This()); \
  v8::Local<v8::Object> optionsObj;                                           \
  v8::Local<v8::Function> callback;                                           \
  if (optionPos == -1 && args[callbackPos]->IsFunction()) {                   \
    callback = args[callbackPos].As<v8::Function>();                          \
  } else if (optionPos != -1 && args[callbackPos - 1]->IsFunction()) {        \
    callback = args[callbackPos - 1].As<v8::Function>();                      \
  } else if (optionPos != -1                                                  \
        && args[optionPos]->IsObject()                                        \
        && args[callbackPos]->IsFunction()) {                                 \
    optionsObj = args[optionPos].As<v8::Object>();                            \
    callback = args[callbackPos].As<v8::Function>();                          \
  } else {                                                                    \
    return NanThrowError(#name "() requires a callback argument");            \
  }

/*!
 * Setup native status bacon.
 */

#define PI_GPIO_SETUP_NATIVE(name)                                            \
  pidaeus::GPIOStatus* status = new GPIOStatus();                             \
  status->success = true;                                                     \
  if (active == false) {                                                      \
    status->success = false;                                                  \
    status->msg = #name "() requires gpio to be setup";                       \
    return status;                                                            \
  }                                                                           \

/*!
 * No V8 macro for erroring out a pin action.
 */

#define PI_GPIO_PIN_HANDLE_NATIVE(pin)                                        \
  if (pins[pin] == NULL) {                                                    \
    std::stringstream msg;                                                    \
    msg << "pin " << pin << " has not been claimed";                          \
    status->success = false;                                                  \
    status->msg = msg.str();                                                  \
    return status;                                                            \
  }                                                                           \
  pi_gpio_handle_t *handle = pins[pin];                                       \

/*!
 * Start namespace
 */

namespace pidaeus {

/*~
 * Status baton for async methods.
 * TODO: generalize for I2C
 */

struct GPIOStatus {
  bool success;
  std::string msg;
};

/*!
 * Constructor method for GPIO javascript object.
 */

NAN_METHOD(GPIOConstruct);

/*!
 * Class GPIO
 *
 * ObjectWrap bridge between CPP and Node
 *
 * @type Class
 * @name GPIO
 * @inherit {ObjectWrap}
 */

class GPIO: public node::ObjectWrap {
  public:
    static void Init();
    static v8::Handle<v8::Value> NewInstance();

    // native bridges
    GPIOStatus* NativeSetup();
    GPIOStatus* NativeTeardown();
    GPIOStatus* NativePinClaim(pi_gpio_pin_t pin, pi_gpio_mode_t mode, pi_gpio_pull_t pull);
    GPIOStatus* NativePinRelease(pi_gpio_pin_t pin);
    GPIOStatus* NativePinRead(pi_gpio_pin_t pin, pi_gpio_value_t& value);
    GPIOStatus* NativePinWrite(pi_gpio_pin_t pin, pi_gpio_value_t value);
    GPIOStatus* NativePinAddListener(pi_gpio_pin_t pin);
    GPIOStatus* NativePinRemoveListener(pi_gpio_pin_t pin);

    // bridge variables
    bool active;
    pi_closure_t *closure;
    pi_gpio_handle_t *pins[PI_MAX_PINS];

    // pin listening
    GpioListener *listeners[PI_MAX_PINS];
    NanCallback *pin_listener;
    GpioListenerQueue *listener_queue;

    // cpp (de)construct methods
    GPIO ();
    ~GPIO ();

  private:

    // methods
    static NAN_METHOD(New);
    static NAN_METHOD(Setup);
    static NAN_METHOD(Teardown);
    static NAN_METHOD(PinClaim);
    static NAN_METHOD(PinRelease);
    static NAN_METHOD(PinRead);
    static NAN_METHOD(PinWrite);
    static NAN_METHOD(PinWriteSync);
    static NAN_METHOD(PinAddListener);
    static NAN_METHOD(PinRemoveListener);

    // getters/setters
    static NAN_GETTER(GetPinListener);
    static NAN_SETTER(SetPinListener);

    // pin listening
    static void EmitPinValue(uv_async_t *async, int status);
};

} // end namespace

#endif
