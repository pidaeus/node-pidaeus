/*!
 * Only once
 */

#ifndef __PI_GPIO_ASYNC_H_
#define __PI_GPIO_ASYNC_H_

/*!
 * External includes
 */

#include <node.h>

/*!
 * Source controlled includes
 */

#include <pi.h>
#include "nan.h"

/*!
 * Local includes
 */

#include "gpio.h"

/**
 * Start namespace
 */

namespace pidaeus {

/*!
 * Abstract worker that wraps up GPIO and callback
 */

class GPIOWorker : public NanAsyncWorker {
  public:
    GPIOWorker(
        pidaeus::GPIO* gpio
      , NanCallback *callback
    ) : NanAsyncWorker(callback), gpio(gpio) {
      NanScope();
      v8::Local<v8::Object> obj = v8::Object::New();
      NanAssignPersistent(v8::Object, persistentHandle, obj);
    };

  protected:
    GPIO* gpio;
    void SetStatus(GPIOStatus *status) {
      if (!status->success) this->errmsg = strdup(status->msg.c_str());
      delete status;
    };
};

/**
 * Async GPIO setup worker
 *
 * @inherits {GPIOWorker}
 */

class SetupWorker : public GPIOWorker {
  public:
    SetupWorker(
        GPIO *gpio
      , NanCallback *callback
    );

    virtual ~SetupWorker();
    virtual void Execute();
};

/**
 * Async GPIO teardown worker
 *
 * @inherits {GPIOWorker}
 */

class TeardownWorker : public GPIOWorker {
  public:
    TeardownWorker(
        GPIO *gpio
      , NanCallback *callback
    );

    virtual ~TeardownWorker();
    virtual void Execute();
};

/**
 * Abstract pin worker that wraps pin into class.
 *
 * @inherits {GPIOWorker}
 */

class PinWorker : public GPIOWorker {
  public:
    PinWorker(
        GPIO *gpio
      , NanCallback *callback
      , pi_gpio_pin_t pin
    );

    virtual ~PinWorker();

  protected:
    pi_gpio_pin_t pin;
};

/**
 * Async GPIO pin claim worker.
 *
 * @inherits {PinWorker}
 */

class PinClaimWorker : public PinWorker {
  public:
    PinClaimWorker(
        GPIO *gpio
      , NanCallback *callback
      , pi_gpio_pin_t pin
      , pi_gpio_direction_t direction
      , pi_gpio_pull_t pull
    );

    virtual ~PinClaimWorker();
    virtual void Execute();

  private:
    pi_gpio_direction_t direction;
    pi_gpio_pull_t pull;
};

/**
 * Async GPIO pin release worker.
 *
 * @inherits {PinWorker}
 */

class PinReleaseWorker : public PinWorker {
  public:
    PinReleaseWorker(
        GPIO *gpio
      , NanCallback *callback
      , pi_gpio_pin_t pin
    );

    virtual ~PinReleaseWorker();
    virtual void Execute();
};

/**
 * Async GPIO pin read worker.
 *
 * @inherits {PinWorker}
 */

class PinReadWorker : public PinWorker {
  public:
    PinReadWorker(
        GPIO *gpio
      , NanCallback *callback
      , pi_gpio_pin_t pin
    );

    virtual ~PinReadWorker();
    virtual void Execute();
    virtual void HandleOKCallback();

  private:
    pi_gpio_value_t value;
};

/**
 * Async GPIO pin write worker.
 *
 * @inherits {PinWorker}
 */

class PinWriteWorker : public PinWorker {
  public:
    PinWriteWorker(
        GPIO *gpio
      , NanCallback *callback
      , pi_gpio_pin_t pin
      , pi_gpio_value_t value
    );

    virtual ~PinWriteWorker();
    virtual void Execute();

  private:
    pi_gpio_value_t value;
};

} // end namespace

#endif
