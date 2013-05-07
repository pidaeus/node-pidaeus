var GPIO_PIN = 4;

describe('GPIO', function () {
  var GPIO = pidaeus.GPIO;

  it('(setup/teardown)', function () {
    var gpio = new GPIO;
    (function () {
      gpio.setup();
      gpio.teardown();
    }).should.not.throw();
  });

  describe('.claim()', function () {
    var gpio;

    beforeEach(function () {
      gpio = new GPIO;
      gpio.setup();
    });

    afterEach(function () {
      gpio.teardown();
      gpio = null;
    });

    it('should claim a pin', function () {
      (function () {
        gpio.claim(GPIO_PIN);
      }).should.not.throw();
    });

    it('should error if claimed again', function () {
      (function () {
        gpio.claim(GPIO_PIN);
      }).should.throw('gpio pin already claimed');
    });
  });
});
