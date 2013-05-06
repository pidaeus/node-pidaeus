
#ifndef __NODE_GPIO_H__
#define __NODE_GPIO_H__

#include <v8.h>
#include <node.h>
#include <pi.h>

#define PI_MAX_PINS 31

using namespace v8;
using namespace node;

class GPIO: public ObjectWrap {
  public:
    static void Initialize(Handle<Object> target);

  private:
    GPIO ();
    ~GPIO ();

    pi_gpio_pin_t pins[PI_MAX_PINS];
    int active;

    static Persistent<FunctionTemplate> constructor;
    static Handle<Value> New(const Arguments &args);
    static Handle<Value> GetEnum(Local<String> prop, const AccessorInfo &info);
    static Handle<Value> Setup(const Arguments &args);
    static Handle<Value> Teardown(const Arguments &args);
    static Handle<Value> ClaimPin(const Arguments &args);
    static Handle<Value> ReleasePin(const Arguments &args);
    static Handle<Value> SetPinDirection(const Arguments &args);
    //static Handle<Value> GetPinDirection(const Arguments &args);
    //static Handle<Value> SetPinPull(const Arguments &args);
    //static Handle<Value> ReadPin(const Arguments &args);
    //static Handle<Value> WritePin(const Arguments &args);
};

#endif
