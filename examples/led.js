var pidaeus = require('..');

var gpio = pidaeus.bind('gpio');
var led = gpio.createWriteStream(18);

led.on('release', gpio.teardown.bind(gpio));

led.write(1, function() {
  setTimeout(function() {
    led.end(0);
  }, 3000);
});
