
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
    debug('create binding');
    var handle = this._handle = new Binding();
    bindEvents(handle, this);
    debug('setup');
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
    this.emit('error', err);
    return this;
  }

  direction = direction || "in";
  this._handle.claim(pin, direction);
  return this;
};

GPIO.prototype.release = function (pin) {
  if (!this._handle) {
    var err = new Error('run setup first');
    this.emit('error', err);
    return this;
  }

  this._handle.release(pin);
  return this;
};

GPIO.prototype.stat = function (pin) {
  if (!this._handle) {
    var err = new Error('run setup first');
    this.emit('error', err);
    return null;
  }

  return this._handle.stat(pin);
};

GPIO.prototype.setDirection = function (pin, direction) {
  if (!this._handle) {
    var err = new Error('run setup first');
    this.emit('error', err);
    return this;
  }

  this._handle.setDirection(pin, direction);
  return this;
};

GPIO.prototype.read = function (pin) {
  if (!this._handle) {
    var err = new Error('run setup first');
    this.emit('error', err);
    return this;
  }

  return this._handle.read(pin);
};

GPIO.prototype.write = function (pin, value) {
  if (!this._handle) {
    var err = new Error('run setup first');
    this.emit('error', err);
    return this;
  }

  value = value || 0;
  return this._handle.write(pin, value);
};

function bindEvents (emitter, target) {
  debug('bindEvents');
  EventEmitter(emitter);
  emitter.bindEvent('ready', target);
  emitter.bindEvent('close', target);
  emitter.bindEvent('error', target);
}
