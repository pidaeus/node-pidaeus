var async = require('breeze-async');
var pidaeus = require('..');

var pins = [ 18, 4, 17, 23, 24 ];

function setStep(g, w1, w2, w3, w4) {
  g.writeSync(pins[1], w1);
  g.writeSync(pins[2], w2);
  g.writeSync(pins[3], w3);
  g.writeSync(pins[4], w4);
}

function move_forward(g, ms, steps, done) {
  var wait = ms / 4;

  function step(i) {
    async.series([
        function(next) {
          setStep(g, 1, 0, 0, 1);
          setTimeout(next, wait);
        }
      , function(next) {
          setStep(g, 0, 1, 0, 1);
          setTimeout(next, wait);
        }
      , function(next) {
          setStep(g, 0, 1, 1, 0);
          setTimeout(next, wait);
        }
      , function(next) {
          setStep(g, 1, 0, 1, 0);
          setTimeout(next, wait);
        }
    ], function() {
      if (i == steps) return done();
      step(++i);
    });
  }

  step(0);
}

var gpio = pidaeus.bind('gpio', function() {
  async.forEach(pins, function(pin, next) {
    gpio.claim(pin, { mode: 1 }, next);
  }, rotate);
});


function rotate() {
  console.log('move forward');
  move_forward(gpio, 12, 356, function() {
    async.forEach(pins, function(pin, next) {
      gpio.release(pin, next);
    }, function() {
      gpio.teardown();
    });
  });
}
