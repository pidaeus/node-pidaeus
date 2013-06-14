var pideaus = require('..')
  , GPIO = pidaeus.GPIO;

var gpio = new GPIO;

var PIN = 23;

gpio.on('close', function () {
  console.log('closing');
  process.exit();
});

console.log('setup...');
gpio.setup(function () {
  console.log('setup done');
  gpio.claim(PIN);
  gpio.write(PIN, 1);
  console.log('fan on');
  setTimeout(function () {
    gpio.write(PIN, 0);
    console.log('fan off');
    gpio.release(PIN);
    console.log('destroying');
    gpio.destroy();
  }, 10000);
});
