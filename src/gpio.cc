
/*!
 * External Includes
 */

#include <v8.h>
#include <node.h>
#include <string.h>
#include <sstream>

/*!
 * Source controlled includes
 */

#include <pi.h>
#include "gpio.h"

/*!
 * Local includes
 */

#include "gpio_async.h"

/*!
 * Start namespace
 */

namespace pidaeus {

/*!
 * v8 Function Template
 */

static v8::Persistent<v8::FunctionTemplate> constructor;

/**
 * Javascript constructor function
 */

NAN_METHOD(GPIOConstruct) {
  NanScope();
  NanReturnValue(GPIO::NewInstance());
}

/**
 * Initialize a new function template.
 */

void
GPIO::Init() {
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(New);
  NanAssignPersistent(v8::FunctionTemplate, constructor, tpl);
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanSymbol("GPIO"));

  // Prototype (Methods)
  NODE_SET_PROTOTYPE_METHOD(tpl, "setup", GPIO::Setup);
  NODE_SET_PROTOTYPE_METHOD(tpl, "teardown", GPIO::Teardown);
  NODE_SET_PROTOTYPE_METHOD(tpl, "claim", GPIO::PinClaim);
  NODE_SET_PROTOTYPE_METHOD(tpl, "release", GPIO::PinRelease);
  NODE_SET_PROTOTYPE_METHOD(tpl, "read", GPIO::PinRead);
  NODE_SET_PROTOTYPE_METHOD(tpl, "write", GPIO::PinWrite);
}

/**
 * Create javascript object
 */

v8::Handle<v8::Value>
GPIO::NewInstance() {
  NanScope();
  v8::Local<v8::FunctionTemplate> handle = NanPersistentToLocal(constructor);
  v8::Local<v8::Object> instance = handle->GetFunction()->NewInstance(0, NULL);
  return instance;
}

/**
 * Constructor for new GPIO instance.
 */

NAN_METHOD(GPIO::New) {
  NanScope();
  GPIO *self = new GPIO();
  self->Wrap(args.This());
  NanReturnValue(args.This());
}

/**
 * Initialize the GPIO interface asyncronously.
 */

NAN_METHOD(GPIO::Setup) {
  NanScope();
  PI_GPIO_SETUP_COMMON(setup, -1, 0)
  SetupWorker* worker = new SetupWorker(gpio, new NanCallback(callback));
  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

/*!
 * Worker handle for setup.
 */

GPIOStatus*
GPIO::NativeSetup() {
  GPIOStatus *status = new GPIOStatus();
  status->success = true;

  if (active == false) {
    if (0 > pi_gpio_setup(closure)) {
      status->success = false;
      status->msg = "gpio setup failed: do you have permissions";
    } else {
      active = true;
    }
  }

  return status;
}

/**
 * Teardown the GPIO interface asyncronously.
 */

NAN_METHOD(GPIO::Teardown) {
  NanScope();
  PI_GPIO_SETUP_COMMON(teardown, -1, 0)
  TeardownWorker* worker = new TeardownWorker(gpio, new NanCallback(callback));
  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

/*!
 * Worker handle for teardown.
 */

GPIOStatus*
GPIO::NativeTeardown() {
  GPIOStatus *status = new GPIOStatus();
  status->success = true;

  if (active != false) {
    for (int i = 0; i < PI_MAX_PINS; i++) {
      if (pins[i] != NULL) {
        pi_gpio_release(pins[i]);
        pins[i] = NULL;
      }
    }

    pi_gpio_teardown(closure);
    active = false;
  }

  return status;
}

/**
 * Claim pin asyncronously.
 */

NAN_METHOD(GPIO::PinClaim) {
  NanScope();
  PI_GPIO_SETUP_COMMON(claim, 1, 2)

  pi_gpio_pin_t gpioPin = args[0]->Int32Value();

  uint32_t gpioDirection = NanUInt32OptionValue(
      optionsObj
    , NanSymbol("direction")
    , 0
  );

  uint32_t gpioPull = NanUInt32OptionValue(
      optionsObj
    , NanSymbol("pull")
    , 0
  );

  PinClaimWorker* worker = new PinClaimWorker(
      gpio
    , new NanCallback(callback)
    , gpioPin
    , static_cast<pi_gpio_direction_t>(gpioDirection)
    , static_cast<pi_gpio_pull_t>(gpioPull)
  );

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

/**
 * Worker handle for pin claim.
 */

GPIOStatus*
GPIO::NativePinClaim(
    pi_gpio_pin_t pin
  , pi_gpio_direction_t direction
  , pi_gpio_pull_t pull)
{
  PI_GPIO_SETUP_NATIVE(claim)

  pi_gpio_handle_t *handle = pi_gpio_claim_with_args(
      closure
    , pin
    , direction
    , pull
  );

  pins[pin] = handle;
  return status;
}

/**
 * Release pin asyncronously.
 */

NAN_METHOD(GPIO::PinRelease) {
  NanScope();
  PI_GPIO_SETUP_COMMON(release, -1, 1)

  pi_gpio_pin_t gpioPin = args[0]->Int32Value();

  PinReleaseWorker* worker = new PinReleaseWorker(
      gpio
    , new NanCallback(callback)
    , gpioPin
  );

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

/**
 * Worker handle for pin release.
 */

GPIOStatus*
GPIO::NativePinRelease(pi_gpio_pin_t pin) {
  PI_GPIO_SETUP_NATIVE(release)
  PI_GPIO_PIN_HANDLE_NATIVE(pin)

  pi_gpio_release(handle);
  pins[pin] = NULL;

  return status;
}

/**
 * Read pin async
 */

NAN_METHOD(GPIO::PinRead) {
  NanScope();
  PI_GPIO_SETUP_COMMON(read, -1, 1)

  pi_gpio_pin_t pin = args[0]->Int32Value();

  PinReadWorker* worker = new PinReadWorker(
      gpio
    , new NanCallback(callback)
    , pin
  );

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

/**
 * Worker handle for read pin
 */

GPIOStatus*
GPIO::NativePinRead(pi_gpio_pin_t pin, pi_gpio_value_t& value) {
  PI_GPIO_SETUP_NATIVE(write)
  PI_GPIO_PIN_HANDLE_NATIVE(pin)

  pi_gpio_direction_t direction = pi_gpio_get_direction(handle);

  if (direction != PI_DIR_IN) {
    std::stringstream msg;
    msg << "pin " << pin << " is not readable";
    status->success = false;
    status->msg = msg.str();
    return status;
  }

  value = pi_gpio_read(handle);

  return status;
}

/**
 * Write pin async
 */

NAN_METHOD(GPIO::PinWrite) {
  NanScope();
  PI_GPIO_SETUP_COMMON(write, -1, 2)

  pi_gpio_pin_t pin = args[0]->Int32Value();
  pi_gpio_value_t value;

  // TODO: error if not 0 or 1
  switch (args[1]->Int32Value()) {
    case 0:
      value = PI_GPIO_LOW;
      break;
    default:
      value = PI_GPIO_HIGH;
      break;
  }

  PinWriteWorker* worker = new PinWriteWorker(
      gpio
    , new NanCallback(callback)
    , pin
    , value
  );

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

/**
 * Worker handle for write pin.
 */

GPIOStatus*
GPIO::NativePinWrite(pi_gpio_pin_t pin, pi_gpio_value_t value) {
  PI_GPIO_SETUP_NATIVE(write)
  PI_GPIO_PIN_HANDLE_NATIVE(pin)

  pi_gpio_direction_t direction = pi_gpio_get_direction(handle);

  if (direction != PI_DIR_OUT) {
    std::stringstream msg;
    msg << "pin " << pin << " is not writable";
    status->success = false;
    status->msg = msg.str();
    return status;
  }

  pi_gpio_write(handle, value);

  return status;
}

/**
 * Class constructor
 */

GPIO::GPIO () {
  active = false;
  closure = pi_closure_new();

  for (int i = 0; i < PI_MAX_PINS; i++) {
    pins[i] = NULL;
  }
};

/**
 * Class deconstructor
 */

GPIO::~GPIO() {
  pi_closure_delete(closure);
  closure = NULL;
};

} // end namespace
