/*!
 * External dependencies
 */

var Binding = require('bindings')('pidaeus.node').GPIO;
var contraption = require('contraption');

/*!
 * Primary export
 */

var State = module.exports = contraption('gpio');

/*!
 * Initial state
 */

State.set('initial', 'close');

/**
 * #### .setup(cb)
 *
 * @param {Function} callback
 * @cb {Error|null} if error
 * @api public
 */

State.method('setup', {
    from: [ 'close' ]
  , during: 'starting'
  , next: 'ready'
  , handle: setup
});

/*!
 * Create binding handle and provy setup
 * method to handle.
 *
 * @param {Object} machine event
 * @param {Function} callback
 * @cb {Error|null} if error
 */

function setup(ev, cb) {
  var self = this;
  var handle = new Binding;
  handle.setup(function(err) {
    if (err) return cb(err);
    self._handle = handle;
    cb();
  });
}

/**
 * #### .teardown(cb)
 *
 * @param {Function} callback
 * @cb {Error|null} if error
 * @api public
 */

State.method('teardown', {
    from: [ 'ready' ]
  , during: 'closing'
  , next: 'close'
  , handle: teardown
});

/*!
 * Proxy teardown to handle then remove handle
 * if success.
 *
 * @param {Object} machine event
 * @param {Function} callback
 * @cb {Error|null} if error
 */

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
