
#ifndef __NODE_GPIO_H__
#define __NODE_GPIO_H__

#include <v8.h>
#include <node.h>
#include <pi.h>

#define PI_MAX_PINS 31

using namespace v8;
using namespace node;

/*!
 * Class GPIO
 *
 * ObjectWrap bridge between CPP and Node
 *
 * @type Class
 * @name GPIO
 * @inherit {ObjectWrap}
 */

class GPIO: public ObjectWrap {
  public:
    static void Initialize(Handle<Object> target);

    // bridge variables
    pi_closure_t *closure;
    pi_gpio_handle_t *pins[PI_MAX_PINS];
    bool active;

  private:
    // cpp (de)construct methods
    GPIO ();
    ~GPIO ();

    // construction
    static Persistent<FunctionTemplate> constructor;
    static Handle<Value> New(const Arguments &args);

    // gpio setup interface
    static Handle<Value> Setup(const Arguments &args);
    static void SetupWork(uv_work_t *req);
    static void SetupAfter(uv_work_t *req, int status);

    // gpio teardown interface
    static Handle<Value> Destroy(const Arguments &args);
    static void DestroyWork(uv_work_t *req);
    static void DestroyAfter(uv_work_t *req, int status);

    // gpio pin management
    static Handle<Value> PinClaimSync(const Arguments &args);
    static Handle<Value> PinReleaseSync(const Arguments &args);

    // gpio pin statistics
    static Handle<Value> PinStat(const Arguments &args);

    // gpio pin modifications
    static Handle<Value> PinSetDirection(const Arguments &args);
    //static Handle<Value> SetPinPull(const Arguments &args);

    // gpio i/o
    static Handle<Value> PinRead(const Arguments &args);
    static Handle<Value> PinWrite(const Arguments &args);
};

/*!
 * Baton structure for use with uv_queue_work.
 *
 * @type struct
 * @name Baton
 * @param {uv_work_t} req
 * @param {v8::Persistent<v8::Function>} cb
 * @param {GPIO*} self
 */

struct Baton {
  uv_work_t req;
  Persistent<Function> cb;
  GPIO* self;
};

#endif
