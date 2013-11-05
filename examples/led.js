var pidaeus = require('..');

var gpio = pidaeus.bind('gpio', function() {
  var led = createLed(gpio, 18);
  var button = createButton(gpio, 22, { holdTime: 500 });

  button.on('hold', function() {
    led.toggle();
  });
});












function createLed(g, p) {
  var led = g.createWriteStream(p);
  var last = 0;

  return {
      on: function(cb) {
        led.write(1, cb);
        last = 1;
      }
    , off: function(cb) {
        led.write(0, cb);
        last = 0;
      }
    , toggle: function(cb) {
        last = last ? 0 : 1;
        led.write(last, cb);
      }
    , done: function(cb) {
        led.once('release', cb);
        led.end(0);
      }
  }
}
