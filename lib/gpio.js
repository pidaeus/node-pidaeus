

var debug = require('sherlock')('pidaeus:gpio')
  , Binding = require('bindings')('pidaeus.node').GPIO
  , EventEmitter = require('events').EventEmitter
  , inherits = require('util').inherits;

module.exports = GPIO;

function GPIO () {
  EventEmitter.call(this);
  this.active = false;
  this._handle = null;
}

inherits(GPIO, EventEmitter);

GPIO.prototype.setup = function (cb) {
  var self = this
    , handle = this._handle;

  function done (err) {
    if (err) return self.onerror(err, cb);
    debug('(setup) success');
    self.active = true;
    cb && cb();
    self.emit('ready');
  }

  if (!handle) {
    debug('(setup) invoke');
    handle = new Binding;
    handle.setup(done);
  } else if (handle && !this.active) {
    debug('(setup) wait for ready');
    this.once('ready', function () {
      debug('(setup) deferred');
      cb && cb();
    });
  } else {
    debug('(setup) immediate');
    setImmediate(done);
  }

  this._handle = handle;
  return this;
};

GPIO.prototype.destroy = function (cb) {
  var self = this
    , handle = this._handle;

  function done (err) {
    if (err) return self.handleError(err, cb);
    debug('(destroy) success');
    self._handle = null;
    self.active = false;
    cb && cb();
    self.emit('close');
  }

  if (handle && self.active) {
    debug('(destroy) invoke');
    handle.destroy(done);
  } else if (handle) {
    debug('(destroy) wait for ready');
    this.once('ready', function () {
      debug('(destroy) invoke');
      handle.destroy(done);
    });
  } else {
    debug('(destroy) immediate');
    setImmediate(done);
  }

  return this;
};

GPIO.prototype.claimSync = function (pin, dir) {
  var err;

  if (!this._handle) {
    err = new Error('run setup first');
    this.onerror(err);
    return this;
  }

  if (!arguments.length || 'number' !== typeof pin) {
    err = new Error('First argument must be a number.');
    this.onerror(err);
    return this;
  }

  dir = dir && dir.toLowerCase() === 'out' ? 'out' : 'in';
  debug('(claimSync) %d - %s', pin, dir);

  try {
    this._handle.claimSync(pin, dir);
  } catch (ex) {
    this.onerror(ex);
  }

  return this;
};

GPIO.prototype.releaseSync = function (pin) {
  var err;

  if (!this._handle) {
    err = new Error('run setup first');
    this.onerror(err);
    return this;
  }

  if (!arguments.length || 'number' !== typeof pin) {
    err = new Error('First argument must be a number.');
    this.onerror(err);
    return this;
  }

  debug('(release) %d', pin);

  try {
    this._handle.releaseSync(pin);
  } catch (ex) {
    this.onerror(ex);
  }

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

GPIO.prototype.onerror = function (err, cb) {
  if (cb && 'function' === typeof cb) {
    debug('error [cb]: %s', err.code || err.message);
    cb(err);
  } else {
    debug('error [emit]: %s', err.code || err.messsage);
    this.emit('error', err);
  }

  return this;
}
