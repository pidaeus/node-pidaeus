/*!
 * External dependencies
 */

var debug = require('sherlock')('pidaeus:gpio:writable');
var inherits = require('util').inherits;
var WritableStream = require('stream').Writable;

/*!
 * Primary export
 */

module.exports = Writable;

/**
 * ### Writable(gpio, pin)
 *
 * Create a writable stream that writes
 * data to pin.
 *
 * @param {GPIO} gpio
 * @param {Number} gpio pin
 * @api public
 */

function Writable(gpio, pin) {
  WritableStream.call(this, { objectMode: true });
  this._claimed = false;
  this._gpio = gpio;
  this._pin = pin;
}

/*!
 * Inherits ReadableStream
 */

inherits(Writable, WritableStream);

/*!
 * WritableStream `._write()` implementation.
 *
 * @param {Buffer|Number} chunk
 * @param {String} encoding
 * @param {Function} callback
 * @api private
 */

Writable.prototype._write = function(chunk, enc, cb) {
  var self = this;
  var gpio = this._gpio;
  var pin = this._pin;

  function write() {
    debug('(write) [%d] %d', pin, chunk);
    gpio.write(pin, chunk, cb);
  }

  function claim() {
    debug('(claim) [%d]', pin);
    gpio.claim(pin, { mode: 1 }, function(err) {
      if (err) return cb(err);
      self._claimed = true;
      self.once('finish', self.onfinish.bind(self));
      self.emit('claim');
      write();
    });
  }

  if ('ready' !== gpio._state.state) {
    debug('(_write) wait');
    gpio.once('ready', claim);
  } else if (!this._claimed) {
    claim();
  } else {
    write();
  }
};

/*!
 * Handle release of pin.
 *
 * TODO: match claim/release patern of readable
 *
 * @api private
 */

Writable.prototype.onfinish = function() {
  var self = this;
  var gpio = this._gpio;
  var pin = this._pin;

  debug('(finish) [%d] release', pin);
  gpio.release(pin, function(err) {
    if (err) return self.emit('error', err);
    self._claimed = false;
    self.emit('release');
  });
};
