/*!
 * External includes
 */

#include <node.h>
#include <uv.h>

/*!
 * Local includes
 */

#include "gpio_async.h"

/*!
 * Start namespace
 */

namespace pidaeus {

/*!
 * Setup Worker
 */

SetupWorker::SetupWorker(
    GPIO *gpio
  , NanCallback *callback
) : GPIOWorker(gpio, callback)
{};

SetupWorker::~SetupWorker() {};

void SetupWorker::Execute() {
  SetStatus(gpio->NativeSetup());
}

/*!
 * Teardown Worker
 */

TeardownWorker::TeardownWorker(
    GPIO *gpio
  , NanCallback *callback
) : GPIOWorker(gpio, callback)
{};

TeardownWorker::~TeardownWorker() {};

void TeardownWorker::Execute() {
  SetStatus(gpio->NativeTeardown());
}

/*!
 * Abstract pin worker
 */

PinWorker::PinWorker(
    GPIO *gpio
  , NanCallback *callback
  , pi_gpio_pin_t pin
) : GPIOWorker(gpio, callback)
  , pin(pin)
{};

PinWorker::~PinWorker() {};

/*!
 * Claim pin worker
 */

PinClaimWorker::PinClaimWorker(
    GPIO *gpio
  , NanCallback *callback
  , pi_gpio_pin_t pin
  , pi_gpio_mode_t mode
  , pi_gpio_pull_t pull
) : PinWorker(gpio, callback, pin)
  , mode(mode)
  , pull(pull)
{};

PinClaimWorker::~PinClaimWorker() {};

void PinClaimWorker::Execute() {
  SetStatus(gpio->NativePinClaim(pin, mode, pull));
}

/*!
 * Release pin worker
 */

PinReleaseWorker::PinReleaseWorker(
    GPIO *gpio
  , NanCallback *callback
  , pi_gpio_pin_t pin
) : PinWorker(gpio, callback, pin)
{};

PinReleaseWorker::~PinReleaseWorker() {};

void PinReleaseWorker::Execute() {
  SetStatus(gpio->NativePinRelease(pin));
}

/*!
 * Read pin worker
 */

PinReadWorker::PinReadWorker(
    GPIO *gpio
  , NanCallback *callback
  , pi_gpio_pin_t pin
) : PinWorker(gpio, callback, pin)
{};

PinReadWorker::~PinReadWorker() {};

void PinReadWorker::Execute() {
  SetStatus(gpio->NativePinRead(pin, value));
}

void PinReadWorker::HandleOKCallback() {
  NanScope();

  v8::Local<v8::Value> argv[]= {
      v8::Local<v8::Value>::New(v8::Null())
    , v8::Number::New(value)
  };

  callback->Call(2, argv);
}

/*!
 * Write pin worker
 */

PinWriteWorker::PinWriteWorker(
    GPIO *gpio
  , NanCallback *callback
  , pi_gpio_pin_t pin
  , pi_gpio_value_t value
) : PinWorker(gpio, callback, pin)
  , value(value)
{};

PinWriteWorker::~PinWriteWorker() {};

void PinWriteWorker::Execute() {
  SetStatus(gpio->NativePinWrite(pin, value));
}

/*!
 *
 */

PinAddListenerWorker::PinAddListenerWorker(
    GPIO *gpio
  , NanCallback *callback
  , pi_gpio_pin_t pin
) : PinWorker(gpio, callback, pin)
{};

PinAddListenerWorker::~PinAddListenerWorker() {};

void PinAddListenerWorker::Execute() {
  SetStatus(gpio->NativePinAddListener(pin));
}

/*!
 *
 */

PinRemoveListenerWorker::PinRemoveListenerWorker(
    GPIO *gpio
  , NanCallback *callback
  , pi_gpio_pin_t pin
) : PinWorker(gpio, callback, pin)
{};

PinRemoveListenerWorker::~PinRemoveListenerWorker() {};

void PinRemoveListenerWorker::Execute() {
  SetStatus(gpio->NativePinRemoveListener(pin));
}

} // end namespace
