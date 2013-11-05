var pidaeus = require('..');

var gpio = pidaeus.bind('gpio', function() {
  gpio._handle._pinListener = function(pin, val) {
    console.log('hello universe', pin, val);
  };

  console.log('add');
  gpio._handle.addPinListener(23, function(err) {
    if (err) throw err;
    console.log('added');
    setTimeout(function() {
      console.log('remove');
      gpio._handle.removePinListener(23, function(err) {
        if (err) console.log(err.message);
        console.log('removed');
        gpio.teardown(function(err) {
          if (err) console.log(err.message);
          console.log('done');
        });
      });
    }, 10000);
  });

});
