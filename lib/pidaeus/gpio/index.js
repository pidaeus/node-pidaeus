/*!
 * External dependencies
 */

var debug = require('sherlock')('pidaeus:gpio');
var sherlock = require('sherlock');

/*!
 * State handle
 */

var State = require('./state');

/*!
 * Streams
 */

var ReadableStream = require('./readable-stream');
var WritableStream = require('./writable-stream');

/*!
 * Primary export
 */

module.exports = GPIO;

/**
 * ### GPIO
 *
 * @return {GPIO}
 * @api public
 */

function GPIO() {
  State.call(this, '_state');
  this._state.debug = sherlock('pidaeus:gpio-state');
  this._handle = null;
}

/*!
 * Mixin state
 */

State.mixin(GPIO.prototype, '_state');

/**
 * #### .claim(pin, [opts], [callback])
 *
 * @param {Number} pin
 * @param {Object} options
 * @param {Function} callback
 * @cb {Error|null} if error
 * @api public
 */

GPIO.prototype.claim = function(pin, opts, cb) {
  if ('function' === typeof opts) cb = opts, opts = {};
  var self = this;
  var handle = this._handle;

  function claim(next) {
    handle.claim(pin, opts, function(err) {
      if (err) return next(err);
      debug('(claimed) [%d] %j', pin, opts);
      self.emit('claim', pin);
      next();
    });
  }

  wrap(this, claim, cb);
};

/**
 * #### .release(pin, [callback])
 *
 * @param {Number} pin
 * @param {Function} callback
 * @cb {Error|null} if error
 * @api public
 */

GPIO.prototype.release = function(pin, cb) {
  var self = this;
  var handle = this._handle;

  function release(next) {
    handle.release(pin, function(err) {
      if (err) return next(err);
      debug('(released) [%d]', pin);
      self.emit('release', pin);
      next();
    });
  }

  wrap(this, release, cb);
};

/**
 * #### .read(pin, [callback])
 *
 * @param {Number} pin
 * @param {Function} callback
 * @cb {Error|null} if error
 * @cb {Number} value
 * @api public
 */

GPIO.prototype.read = function(pin, cb) {
  var self = this;
  var handle = this._handle;

  function read(next) {
    handle.read(pin, function(err, value) {
      if (err) return next(err);
      debug('(read) [%d] %d', pin, value);
      next(null, value);
    });
  }

  wrap(this, read, cb);
};

/**
 * #### .write(pin, value, [callback])
 *
 * @param {Number} pin
 * @param {Number} value
 * @param {Function} callback
 * @cb {Error|null} if error
 * @api public
 */

GPIO.prototype.write = function(pin, value, cb) {
  var self = this;
  var handle = this._handle;

  function write(next) {
    handle.write(pin, value, function(err) {
      if (err) return next(err);
      debug('(write) [%d] %d', pin, value);
      next();
    });
  }

  wrap(this, write, cb);
};

GPIO.prototype.writeSync = function(pin, value) {
  var self = this;
  var handle = this._handle;

  function writeSync() {
    handle.writeSync(pin, value);
    debug('(write) [%d] %d', pin, value);
  }

  return wrapSync(this, writeSync);
};

/**
 * #### .createWriteStream(pin)
 *
 * @param {Number} pin
 * @return {WritableStream}
 * @api public
 */

GPIO.prototype.createReadStream = function(pin, interval, pull) {
  return new ReadableStream(this, pin, interval, pull);
};

/**
 * #### .createWriteStream(pin)
 *
 * @param {Number} pin
 * @return {WritableStream}
 * @api public
 */

GPIO.prototype.createWriteStream = function(pin) {
  return new WritableStream(this, pin);
};

/*!
 * Wrap non-state functions to error if
 * gpio interface not ready.
 *
 * @param {GPIO} self context
 * @param {Function} handle if pass
 * @param {Function} callback on done
 * @api private
 */

function wrap(self, fn, cb) {
  var state = self._state.state;

  function done() {
    var args = [].slice.call(arguments);
    var err = args.shift();
    if (cb && err) return cb(err);
    if (err) return self.emit('error', err);
    if (cb) cb.apply(self, [ null ].concat(args));
  }

  if ('ready' !== state) {
    var err = new Error('interface not ready');
    return setImmediate(done.bind(null, err));
  }

  fn(done);
}

function wrapSync(self, fn) {
  var state = self._state.state;

  function done(err, val) {
    if (err) throw err;
    return val;
  }

  if ('ready' !== state) {
    var err = new Error('interface not ready');
    return done(err);
  }

  try {
    var res = fn();
  } catch (ex) {
    return done(err);
  }

  return done(null, res);
}
