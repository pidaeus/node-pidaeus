/*!
 * External dependencies
 */

var debug = require('sherlock')('pidaeus:gpio:readable');
var inherits = require('util').inherits;
var ReadableStream = require('stream').Readable;

/*!
 * Primary export
 */

module.exports = Readable;

/**
 * ### Readable(gpio, pin, interval, pull)
 *
 * Create a readable stream that emits data
 * events at a given interval.
 *
 * @param {GPIO} gpio
 * @param {Number} gpio pin
 * @param {Number} poll interval
 * @param {Number} pull option (default: `0`)
 * @api public
 */

function Readable(gpio, pin, interval, pull) {
  ReadableStream.call(this, { objectMode: true });
  this._claimed = false;
  this._finished = false;
  this._gpio = gpio;
  this._interval = interval || 1000;
  this._options = { mode: 0 , pull: pull || 0 };
  this._pin = pin;
  this._wait = false;

  if ('ready' !== gpio._state.state) {
    gpio.once('ready', this._claim.bind(this));
  } else {
    this._claim();
  }
}

/*!
 * Inherits ReadableStream
 */

inherits(Readable, ReadableStream);

/**
 * #### .close([cb])
 *
 * Stop polling for current data value
 * and release the pin.
 *
 * @param {Function} callback
 * @cb {Error|null} if error
 * @api public
 */

Readable.prototype.close = function(cb) {
  var self = this;

  function success() {
    self.removeListener('close', success);
    self.removeListener('error', error);
    cb();
  }

  function error(err) {
    self.removeListener('close', success);
    self.removeListener('error', error);
    cb(err);
  }

  if (cb && 'function' === typeof cb) {
    self.on('close', success);
    self.on('error', error);
  }

  this._finished = true;
  this.emit('_nudge');
};

/*!
 * ReadableStream `._read()` implemntation.
 *
 * @api private
 */

Readable.prototype._read = function() {
  var self = this;
  var gpio = this._gpio;
  var pin = this._pin;
  var wto;

  function cancel() {
    clearTimeout(wto);
  }

  function wait() {
    self._wait = true;
    self.on('end', cancel);

    wto = setTimeout(function() {
      self.removeListener('end', cancel);
      self._wait = false;
      self.emit('_nudge');
    }, self._interval);
  }

  function read() {
    if (self._finished) return self.push(null);
    gpio.read(pin, function(err, value) {
      if (err) return self.emit('error', err);
      wait();
      self.push(value);
    });
  }

  if (!self._claimed) {
    debug('(_read) [%d] waiting on claim', pin);
    this.once('_nudge', read);
  } else if (self._wait) {
    debug('(_read) [%d] waiting on nudge', pin);
    this.once('_nudge', read);
  } else {
    read();
  }
};

/*!
 * Handle claim of pin and listening for events
 * that should trigger pin release and stream
 * `close`.
 *
 * @api private
 */

Readable.prototype._claim = function() {
  var self = this;
  var gpio = this._gpio;
  var pin = this._pin;

  function finish() {
    cleanup();
    self._finished = true;
    self.once('end', closed);
    self._emit('_nudge');
  }

  function cleanup() {
    gpio.removeListener('close', close);
    self.removeListener('end', release);
    self._claimed = false;
  }

  function release() {
    cleanup();
    debug('(release) %d', pin);
    gpio.release(pin, closed);
  }

  function closed(err) {
    if (err) return self.emit('error', err);
    self.emit('release');
    self.emit('close');
  }

  if (this._finished) {
    debug('(claim) [%d] finished before claim', pin);
    self.emit('_nudge');
    return;
  }

  debug('(claim) [%d]', pin);
  gpio.claim(pin, this._options, function(err) {
    if (err) return self.emit('error', err);
    self._claimed = true;
    self.on('end', release);
    gpio.on('close', finish);
    self.emit('claim');
    self.emit('_nudge');
  });
};
