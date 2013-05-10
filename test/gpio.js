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
      it('should claim a pin with default options', function (done) {
        var gpio = new GPIO;
        gpio.setup(function () {
          (function () {
            gpio.claim(GPIO_PIN);
          }).should.not.throw();
          gpio.destroy(done);
        });
      });

      it('should claim a pin with direction = "in"', function (done) {
        var gpio = new GPIO;
        gpio.setup(function () {
          (function () {
            gpio.claim(GPIO_PIN, 'in');
          }).should.not.throw();
          gpio.destroy(done);
        });
      });

      it('should claim a pin with direction = "out"', function (done) {
        var gpio = new GPIO;
        gpio.setup(function () {
          (function () {
            gpio.claim(GPIO_PIN, 'out');
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

    describe('.stat()', function () {
      it('should return an object', function (done) {
        var gpio = new GPIO
          , stat;

        gpio.setup(function () {
          gpio.claim(GPIO_PIN);
          stat = gpio.stat(GPIO_PIN);
          gpio.release(GPIO_PIN);
          should.exist(stat);
          stat.should.have.property('pin')
            .a('number').equal(GPIO_PIN);
          stat.should.have.property('claimed')
            .a('boolean').equal(true);
          stat.should.have.property('direction')
            .a('string').equal('in');
          gpio.destroy(done);
        });
      });
    });

    describe('.setDirection()', function () {
      it('should set direction of pin', function (done) {
        var gpio = new GPIO
          , stat;

        gpio.setup(function () {
          gpio.claim(GPIO_PIN);
          stat = gpio.stat(GPIO_PIN);
          stat.should.have.property('direction', 'in');
          gpio.setDirection(GPIO_PIN, "out");
          stat = gpio.stat(GPIO_PIN);
          gpio.release(GPIO_PIN);
          stat.should.have.property('direction', 'out');
          gpio.destroy(done);
        });
      });
    });

    describe('.read()', function () {
      it('should read a pin\'s value', function (done) {
        var gpio = new GPIO
          , val;

        gpio.setup(function () {
          gpio.claim(GPIO_PIN, 'in');
          (function () {
            val = gpio.read(GPIO_PIN);
          }).should.not.throw();
          val.should.be.a('number').equal(0);
          gpio.destroy(done);
        });
      });
    });

    describe('.write()', function () {
      it('should write a value to a pin', function (done) {
        var gpio = new GPIO;

        gpio.setup(function () {
          gpio.claim(GPIO_PIN, 'out');
          (function () {
            gpio.write(GPIO_PIN, 1);
          }).should.not.throw();
          gpio.destroy(done);
        });
      });
    });
  });
});
