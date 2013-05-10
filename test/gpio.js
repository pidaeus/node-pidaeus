var GPIO_PIN = 4;

describe('GPIO', function () {
  var GPIO = pidaeus.GPIO;

  describe('(setup/destroy)', function () {
    it('should emit proper events', function (done) {
      var gpio = new GPIO
        , errSpy = chai.spy('error')
        , readySpy = chai.spy('ready', function () {
            console.log('ready');
            gpio.destroy();
          });

      gpio.on('ready', readySpy);
      gpio.on('close', function () {
        errSpy.should.not.have.been.called;
        readySpy.should.have.been.called.once;
        done();
      });

      gpio.setup();
    });
  });

  describe('Pins', function () {
    describe('.claim()', function () {
      it('should claim a pin', function (done) {
        var gpio = new GPIO;
        gpio.setup(function () {
          (function () {
            gpio.claim(GPIO_PIN);
          }).should.not.throw();
          gpio.destroy(done);
        });
      });
    });

    describe('.release()', function () {
      it('should release a pin', function (done) {
        var gpio = new GPIO;
        gpio.setup(function () {
          gpio.claim(GPIO_PIN);
          (function () {
            gpio.release(GPIO_PIN);
          }).should.not.throw();
          gpio.destroy(done);
        });
      });
    });
  });
});
