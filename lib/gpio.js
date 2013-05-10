
var debug = require('sherlock')('pidaeus:gpio')
  , Binding = require('bindings')('pidaeus.node').GPIO
  , EventEmitter = require('drip').EventEmitter
  , inherits = require('util').inherits;

module.exports = GPIO;

function GPIO () {
  EventEmitter.call(this);
  this._handle = null;
}

inherits(GPIO, EventEmitter);

GPIO.prototype.setup = function (cb) {
  if (!this._handle) {
    if (cb) this.once('ready', cb);
    var handle = this._handle = new Binding;
    bindEvents(handle, this);
    handle.setup();
  }

  return this;
};

GPIO.prototype.destroy = function (cb) {
  var self = this
    , handle = this._handle;

  if (handle) {
    this.once('close', function () {
      self._handle = null;
      if (cb) cb.call(self);
    });
    this._handle.destroy();
  }

  return this;
};

GPIO.prototype.claim = function (pin, direction) {
  if (!this._handle) {
    var err = new Error('run setup first');
    return this.emit('error', err);
  }

  direction = direction || "in";
  return this._handle.claim(pin, direction);
};

GPIO.prototype.release= function (pin) {
  if (!this._handle) {
    var err = new Error('run setup first');
    return this.emit('error', err);
  }

  return this._handle.release(pin);
};

function bindEvents (emitter, target) {
  EventEmitter(emitter);
  emitter.bindEvent('ready', target);
  emitter.bindEvent('close', target);
  emitter.bindEvent('error', target);
}
