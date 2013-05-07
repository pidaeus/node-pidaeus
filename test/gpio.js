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
      gpio.claim(GPIO_PIN);
      (function () {
        gpio.claim(GPIO_PIN);
      }).should.throw('gpio pin already claimed');
    });
  });

  describe('.stat(pin)', function () {
    it('should return an object', function () {
      var gpio = new GPIO;
      gpio.stat(GPIO_PIN).should.be.an('object');
    });

    it('should return claim status', function () {
      var gpio = new GPIO;
      gpio.setup();
      gpio.stat(GPIO_PIN).should.have.property('claimed', false);
      gpio.claim(GPIO_PIN);
      gpio.stat(GPIO_PIN).should.have.proeprty('claimed', true);
      gpio.release(GPIO_PIN);
      gpio.teardown();
    });
  });
});
