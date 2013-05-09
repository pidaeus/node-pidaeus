
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
    var handle = this._handle = new Binding;
    bindEvents(handle, this);
    this.once('ready', cb);
    handle.setup();
  }

  return this;
};

GPIO.prototype.destroy = function (cb) {
  var handle = this._handle;

  if (handle) {
    this.once('close', cb);
    this._handle.destroy();
  }

  return this;
};

function bindEvents (emitter, target) {
  EventEmitter(emitter);
  emitter.bindEvent('ready', target);
  emitter.bindEvent('close', target);
  emitter.bindEvent('error', target);
}
