
var Binding = require('bindings')('pidaeus.node').GPIO;
var contraption = require('contraption');

var State = module.exports = contraption('gpio');

State.set('initial', 'close');

State.method('setup', {
    from: [ 'close' ]
  , during: 'starting'
  , next: 'ready'
  , handle: setup
});

function setup(ev, cb) {
  var self = this;
  var handle = new Binding;
  handle.setup(function(err) {
    if (err) return cb(err);
    self._handle = handle;
    cb();
  });
}

State.method('teardown', {
    from: [ 'ready' ]
  , during: 'closing'
  , next: 'close'
  , handle: teardown
});

function teardown(ev, cb) {
  var self = this;
  var handle = this._handle;
  handle.teardown(function(err) {
    delete self._handle;
    self._handle = null;
    if (err) return cb(err);
    cb();
  });
}
