
var async = require('breeze-async');
var debug = require('sherlock')('pidaeus:gpio:writable');
var inherits = require('util').inherits;
var WritableStream = require('stream').Writable;

module.exports = Writable;

function Writable(gpio, pin) {
  WritableStream.call(this, { objectMode: true });
  this._claimed = false;
  this._gpio = gpio;
  this._pin = pin;
}

inherits(Writable, WritableStream);

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
    gpio.claim(pin, { direction: 1 }, function(err) {
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
