
exports.GPIO = require('./pidaeus/gpio');

exports.types = {
  'gpio': exports.GPIO
};

exports.bind = function(type, ready) {
  if (!exports.types[type]) {
    throw new Error('invalid interface type "' + type + '"');
  }

  var fn = exports.types[type];
  var iface = new fn();

  if (ready) iface.on('ready', ready);
  setImmediate(iface.setup.bind(iface));
  return iface;
};
