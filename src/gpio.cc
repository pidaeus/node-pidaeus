
#include "gpio.h"


#include <v8.h>
#include <node.h>
#include <pi.h>

using namespace v8;
using namespace node;

#define ERROR(msg) \
  ThrowException(Exception::Error(String::New(msg))

#define TYPE_ERROR(msg) \
  ThrowException(Exception::TypeError(String::New(msg)))

Persistent<FunctionTemplate> GPIO::constructor;

void
GPIO::Initialize(Handle<Object> target) {
  HandleScope scope;

  // Constructor
  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
  constructor = Persistent<FunctionTemplate>::New(tpl);
  constructor->InstanceTemplate()->SetInternalFieldCount(1);
  constructor->SetClassName(String::NewSymbol("GPIO"));

  // Prototype (Methods)
  SetPrototypeMethod(constructor, "setup", Setup);
  SetPrototypeMethod(constructor, "teardown", Teardown);
  SetPrototypeMethod(constructor, "claim", ClaimPin);
  SetPrototypeMethod(constructor, "release", ReleasePin);
  SetPrototypeMethod(constructor, "setDirection", SetPinDirection);
  //SetPrototypeMethod(constructor, "getDirection", GetPinDirection);
  //SetPrototypeMethod(constructor, "setPull", SetPinPull);
  //SetPrototypeMethod(constructor, "read", ReadPin);
  //SetPrototypeMethod(constructor, "write", WritePin);

  // Prototype (Getters/Setters)
  // Local<ObjectTemplate> proto = constructor->PrototypeTemplate();
  //proto->SetAccessor(String::NewSymbol("active"), IsSetupGpio);
  //proto->SetAccessor(String::NewSymbol("INPUT"), GetEnum);
  //proto->SetAccessor(String::NewSymbol("OUTPUT"), GetEnum);

  // Export
  target->Set(String::NewSymbol("GPIO"), constructor->GetFunction());
}

Handle<Value>
GPIO::New(const Arguments &args) {
  HandleScope scope;
  GPIO *self = new GPIO();
  self->Wrap(args.This());
  return scope.Close(args.Holder());
}

/*
Handle<Value>
GPIO::GetEnum(Local<String> prop, const AccessorInfo &info) {
  HandleScope scope;
  int num;

  switch (prop) {
    case "INPUT";
      num = PI_DIR_IN;
      break;
    case "OUTPUT"
      num = PI_DIR_OUT;
      break;
  }

  return scope.Close(Number::New(num));
}
*/

Handle<Value>
GPIO::Setup(const Arguments &args) {
  HandleScope scope;
  GPIO *self = ObjectWrap::Unwrap<GPIO>(args.Holder());

  if (!self->active) {
    int success = pi_gpio_setup();
    if (success < 0) return ERROR("gpio setup failed: are you root?");
    self->active = 1;
  }

  return scope.Close(args.Holder());
}

Handle<Value>
GPIO::Teardown(const Arguments &args) {
  HandleScope scope;
  GPIO *self = ObjectWrap::Unwrap<GPIO>(args.Holder());

  // release all pins first.
  if (self->active) {
    pi_gpio_teardown();
    self->active = 0;
  }

  return scope.Close(args.Holder());
}

Handle<Value>
GPIO::ClaimPin(const Arguments &args) {
  HandleScope scope;
  GPIO *self = ObjectWrap::Unwrap<GPIO>(args.Holder());

  int len = args.Length();
  if (len < 1) return TYPE_ERROR("gpio pin required");
  if (!args[0]->IsUint32()) return TYPE_ERROR("gpio pin must be a number");

  pi_gpio_pin_t gpio = args[0]->Int32Value();
  if (self->pins[gpio]) return ERROR("gpio pin already claimed");

  pi_gpio_handle_t *handle = pi_gpio_claim(gpio);
  self->pins[gpio] = handle;

  return scope.Close(args.Holder());
}

Handle<Value>
GPIO::ReleasePin(const Arguments &args) {
  HandleScope scope;
  GPIO *self = ObjectWrap::Unwrap<GPIO>(args.Holder());

  int len = args.Length();
  if (len < 1) return TYPE_ERROR("gpio pin required");
  if (!args[0]->IsUint32()) return TYPE_ERROR("gpio pin must be a number");

  pi_gpio_pin_t gpio = args[0]->Int32Value();
  if (!self->pins[gpio]) return ERROR("gpio pin has not been claimed");

  pi_gpio_handle_t *handle = self->pins[gpio];
  int res = pi_gpio_release(handle);

  // TODO: Error checking
  return scope.Close(args.Holder());
}

Handle<Value>
GPIO::SetPinDirection(const Arguments &args) {
  HandleScope scope;
  GPIO *self = ObjectWrap::Unwrap<GPIO>(args.Holder());

  int len = args.Length();
  if (len < 1) return TYPE_ERROR("gpio pin required");
  if (!args[0]->IsUint32()) return TYPE_ERROR("gpio pin must be a number");
  if (len < 2) return TYPE_ERROR("gpio direction required");
  if (!args[1]->IsUint32()) return TYPE_ERROR("gpio direction must be a number");

  pi_gpio_pin_t gpio = args[0]->Int32Value();
  if (!self->pins[gpio]) return ERROR("gpio pin has not been claimed");

  pi_gpio_handle_t *handle = self->pins[gpio];
  pi_gpio_direction_t direction = args[1]->Int32Value();
  pi_gpio_set_direction(gpio, direction);

  // TODO: Error checking
  return scope.Close(args.Holder());
}

GPIO::GPIO () {
  active = 0;
};

GPIO::~GPIO() {};
