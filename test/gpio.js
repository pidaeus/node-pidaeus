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
    it('should return valid object for unclaimed pin', function () {
      var gpio = new GPIO
        , stat = gpio.stat(GPIO_PIN);
      should.exist(stat);
      stat.should.be.an('object');
      stat.should.have.property('pin')
        .a('number').equal(GPIO_PIN);
      stat.should.have.property('claimed')
        .a('boolean').equal(false);
    });

    it('should return claim status', function () {
      var gpio = new GPIO
        , stat;
      gpio.setup();
      gpio.claim(GPIO_PIN);
      stat = gpio.stat(GPIO_PIN);

      should.exist(stat);
      stat.should.be.an('object');
      stat.should.have.property('pin')
        .a('number').equal(GPIO_PIN);
      stat.should.have.property('claimed')
        .a('boolean').equal(true);

      gpio.release(GPIO_PIN);
      gpio.teardown();
    });
  });
});
