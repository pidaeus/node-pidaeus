
#include "gpio.h"

#include <v8.h>
#include <node.h>
#include <string.h>

#include <pi.h>

using namespace v8;
using namespace node;

#define ERROR(msg) \
  ThrowException(Exception::Error(String::New(msg)))

#define TYPE_ERROR(msg) \
  ThrowException(Exception::TypeError(String::New(msg)))

static Persistent<String> emit_sym;

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
  SetPrototypeMethod(constructor, "destroy", Destroy);
  SetPrototypeMethod(constructor, "claim", PinClaim);
  SetPrototypeMethod(constructor, "release", PinRelease);
  SetPrototypeMethod(constructor, "stat", PinStat);
  SetPrototypeMethod(constructor, "setDirection", PinSetDirection);
  //SetPrototypeMethod(constructor, "getDirection", GetPinDirection);
  //SetPrototypeMethod(constructor, "setPull", SetPinPull);
  //SetPrototypeMethod(constructor, "read", ReadPin);
  //SetPrototypeMethod(constructor, "write", WritePin);

  // Prototype (Getters/Setters)
  // Local<ObjectTemplate> proto = constructor->PrototypeTemplate();

  // Persistent Symbols
  emit_sym = NODE_PSYMBOL("emit");

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

pi_gpio_direction_t
PiDirection(const Handle<String> &v8str) {
  String::AsciiValue str(v8str);
  if (!strcasecmp(*str, "in")) return PI_DIR_IN;
  if (!strcasecmp(*str, "out")) return PI_DIR_OUT;
  return PI_DIR_IN;
}

Handle<Value>
GPIO::Setup(const Arguments &args) {
  HandleScope scope;
  GPIO *self = ObjectWrap::Unwrap<GPIO>(args.Holder());

  Baton *baton = new Baton();
  baton->req.data = baton;
  baton->object = args.Holder();
  baton->self = self;

  uv_queue_work(
    uv_default_loop(),
    &baton->req,
    SetupWork,
    (uv_after_work_cb)SetupAfter
  );

  return Undefined();
}

void
GPIO::SetupWork(uv_work_t *req) {
  Baton* baton = static_cast<Baton*>(req->data);
  GPIO* self = static_cast<GPIO*>(baton->self);

  if (!self->active) {
    pi_closure_t *closure = pi_closure_new();
    int success = pi_gpio_setup(closure);

    if (success < 0) {
      pi_closure_delete(closure);
      //return ERROR("gpio setup failed: are you root?");
    }

    self->closure = closure;
    self->active = true;
  }
}

void
GPIO::SetupAfter(uv_work_t *req, int status) {
  Baton* baton = static_cast<Baton*>(req->data);

  Local<Value> argv[1] = {
    String::New("ready")
  };

  MakeCallback(baton->object, emit_sym, 1, argv);
  delete baton;
}


Handle<Value>
GPIO::Destroy(const Arguments &args) {
  HandleScope scope;
  GPIO *self = ObjectWrap::Unwrap<GPIO>(args.Holder());

  Baton *baton = new Baton();
  baton->req.data = baton;
  baton->object = args.Holder();
  baton->self = self;

  uv_queue_work(
    uv_default_loop(),
    &baton->req,
    DestroyWork,
    (uv_after_work_cb)DestroyAfter
  );

  return Undefined();
}

void
GPIO::DestroyWork(uv_work_t *req) {
  Baton* baton = static_cast<Baton*>(req->data);
  GPIO* self = static_cast<GPIO*>(baton->self);

  if (self->active) {
    for (int i = 0; i < PI_MAX_PINS; i++) {
      if (self->pins[i] != NULL) {
        pi_gpio_release(self->pins[i]);
        self->pins[i] = NULL;
      }
    }

    pi_gpio_teardown(self->closure);
    pi_closure_delete(self->closure);
    self->closure = NULL;
    self->active = false;
  }
}

void
GPIO::DestroyAfter(uv_work_t *req, int status) {
  Baton* baton = static_cast<Baton*>(req->data);

  Local<Value> argv[1] = {
    String::New("close")
  };

  MakeCallback(baton->object, emit_sym, 1, argv);
  delete baton;
}

Handle<Value>
GPIO::PinClaim(const Arguments &args) {
  HandleScope scope;
  GPIO *self = ObjectWrap::Unwrap<GPIO>(args.Holder());

  int len = args.Length();
  pi_gpio_direction_t direction = PI_DIR_IN;

  if (len < 1) return TYPE_ERROR("gpio pin required");
  if (!args[0]->IsUint32()) return TYPE_ERROR("gpio pin must be a number");

  pi_gpio_pin_t gpio = args[0]->Int32Value();
  if (self->pins[gpio] != NULL) return ERROR("gpio pin already claimed");

  if (len > 1) {
    if (!args[1]->IsString()) return TYPE_ERROR("direction must be a string");
    direction = PiDirection(args[1]->ToString());
  }

  pi_gpio_handle_t *handle = pi_gpio_claim(self->closure, gpio);
  self->pins[gpio] = handle;

  if (direction != PI_DIR_IN) {
    pi_gpio_set_direction(handle, direction);
  }

  return scope.Close(args.Holder());
}


Handle<Value>
GPIO::PinRelease(const Arguments &args) {
  HandleScope scope;
  GPIO *self = ObjectWrap::Unwrap<GPIO>(args.Holder());

  int len = args.Length();
  if (len < 1) return TYPE_ERROR("gpio pin required");
  if (!args[0]->IsUint32()) return TYPE_ERROR("gpio pin must be a number");

  pi_gpio_pin_t gpio = args[0]->Int32Value();
  if (self->pins[gpio] == NULL) return ERROR("gpio pin has not been claimed");

  pi_gpio_handle_t *handle = self->pins[gpio];
  pi_gpio_release(handle);
  self->pins[gpio] = NULL;

  // TODO: Error checking
  return scope.Close(args.Holder());
}

Handle<Value>
GPIO::PinStat(const Arguments &args) {
  HandleScope scope;
  GPIO *self = ObjectWrap::Unwrap<GPIO>(args.Holder());

  int len = args.Length();
  if (len < 1) return TYPE_ERROR("gpio pin required");
  if (!args[0]->IsUint32()) return TYPE_ERROR("gpio pin must be a number");

  Local<Object> res = Object::New();
  pi_gpio_pin_t gpio = args[0]->Int32Value();
  bool exist = self->pins[gpio] == NULL ? false : true;
  char *direction;

  res->Set(String::New("pin"), Number::New(gpio));
  res->Set(String::New("claimed"), Boolean::New(exist));

  if (exist) {
    pi_gpio_handle_t *handle = self->pins[gpio];
    direction = pi_gpio_get_direction(handle);
    char str[120];

    switch (direction) {
      case PI_DIR_IN:
        strcpy(str, "in");
        break;
      case PI_DIR_OUT:
        strcpy(str, "out");
        break;
    }

    res->Set(String::New("direction"), String::New(str));
  } else {
    res->Set(String::New("direction"), Null());
  }

  return scope.Close(res);
}

/*
char *get(v8::Local<v8::Value> value, const char *fallback = "") {
  if (value->IsString()) {
      v8::String::AsciiValue string(value);
      char *str = (char *) malloc(string.length() + 1);
      strcpy(str, *string);
      return str;
  }
  char *str = (char *) malloc(strlen(fallback) + 1);
  strcpy(str, fallback);
  return str;
}
*/

Handle<Value>
GPIO::PinSetDirection(const Arguments &args) {
  HandleScope scope;
  GPIO *self = ObjectWrap::Unwrap<GPIO>(args.Holder());

  int len = args.Length();
  if (len < 1) return TYPE_ERROR("gpio pin required");
  if (!args[0]->IsUint32()) return TYPE_ERROR("gpio pin must be a number");
  if (len < 2) return TYPE_ERROR("gpio direction required");
  if (!args[1]->IsString()) return TYPE_ERROR("gpio direction must be a number");

  pi_gpio_pin_t gpio = args[0]->Int32Value();
  if (!self->pins[gpio]) return ERROR("gpio pin has not been claimed");

  pi_gpio_handle_t *handle = self->pins[gpio];
  pi_gpio_direction_t direction = PiDirection(args[1]->ToString());
  pi_gpio_set_direction(handle, direction);

  // TODO: Error checking
  return scope.Close(args.Holder());
}

GPIO::GPIO () {
  active = false;
  closure = NULL;

  for (int i = 0; i < PI_MAX_PINS; i++) {
    pins[i] = NULL;
  }
};

GPIO::~GPIO() {};
