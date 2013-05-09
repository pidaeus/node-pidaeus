var GPIO_PIN = 4;

describe('GPIO', function () {
  var GPIO = pidaeus.GPIO;

  describe('(setup/destroy)', function () {
    it('should emit proper events', function (done) {
      var gpio = new GPIO
        , errSpy = chai.spy('error')
        , readySpy = chai.spy('ready', function () {
            gpio.destroy();
          });

      gpio.on('close', function () {
        errSpy.should.not.have.been.called;
        readySpy.should.have.been.called.once;
        done();
      });

      gpio.setup();
    });
  });
});
