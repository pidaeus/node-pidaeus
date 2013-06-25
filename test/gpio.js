var GPIO_PIN = 4;

describe('GPIO', function () {
  var GPIO = pidaeus.GPIO;

  function setup(cb) {
    var gpio = new GPIO;
    gpio.setup(function (err) {
      should.not.exist(err);
      cb(gpio, function (done) {
        gpio.destroy(function (err) {
          should.not.exist(err);
          done();
        });
      });
    });
  }

  describe('(setup/destroy)', function () {
    it('should invoke callbacks', function (done) {
      var gpio = new GPIO;

      gpio.setup(function (err) {
        should.not.exist(err);
        gpio.destroy(function (err) {
          should.not.exist(err);
          done();
        });
      });
    });

    it('should emit proper events', function (done) {
      var gpio = new GPIO
        , errSpy = chai.spy('error')
        , readySpy = chai.spy('ready', function () {
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

  describe.only('(pin management)', function () {
    describe('.claimSync()', function () {
      it('should claimSync a pin with default options', function (done) {
        setup(function (gpio, destroy) {
          (function () {
            gpio.claimSync(GPIO_PIN);
          }).should.not.throw();
          destroy(done);
        });
      });

      it('should claimSync a pin with direction = "in"', function (done) {
        setup(function (gpio, destroy) {
          (function () {
            gpio.claimSync(GPIO_PIN, 'in');
          }).should.not.throw();
          destroy(done);
        });
      });

      it('should claimSync a pin with direction = "out"', function (done) {
        setup(function (gpio, destroy) {
          (function () {
            gpio.claimSync(GPIO_PIN, 'out');
          }).should.not.throw();
          destroy(done);
        });
      });

      it('should throw error if already claimed', function (done) {
        setup(function (gpio, destroy) {
          (function () {
            gpio.claimSync(GPIO_PIN);
          }).should.not.throw();
          (function () {
            gpio.claimSync(GPIO_PIN);
          }).should.throw(/already claimed/);
          destroy(done);
        });
      });
    });

    describe('.releaseSync()', function () {
      it('should release a pin', function (done) {
        setup(function (gpio, destroy) {
          gpio.claimSync(GPIO_PIN);
          (function () {
            gpio.releaseSync(GPIO_PIN);
          }).should.not.throw();
          destroy(done);
        });
      });

      it('should throw if not claimed', function (done) {
        setup(function (gpio, destroy) {
          (function () {
            gpio.releaseSync(GPIO_PIN);
          }).should.throw(/not been claimed/);
          destroy(done);
        });
      });
    });
  });

  describe('(io)', function () {
    describe('.stat()', function () {
      it('should return an object', function (done) {
        var gpio = new GPIO
          , stat;

        gpio.setup(function () {
          gpio.claimSync(GPIO_PIN);
          stat = gpio.stat(GPIO_PIN);
          gpio.releaseSync(GPIO_PIN);
          should.exist(stat);
          stat.should.have.property('pin')
            .a('number').equal(GPIO_PIN);
          stat.should.have.property('claimSynced')
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
          gpio.claimSync(GPIO_PIN);
          stat = gpio.stat(GPIO_PIN);
          stat.should.have.property('direction', 'in');
          gpio.setDirection(GPIO_PIN, "out");
          stat = gpio.stat(GPIO_PIN);
          gpio.releaseSync(GPIO_PIN);
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
          gpio.claimSync(GPIO_PIN, 'in', "up");
          (function () {
            val = gpio.read(GPIO_PIN);
          }).should.not.throw();
          val.should.be.a('number').equal(1);
          gpio.destroy(done);
        });
      });
    });

    describe('.write()', function () {
      it('should write a value to a pin', function (done) {
        var gpio = new GPIO;

        gpio.setup(function () {
          gpio.claimSync(GPIO_PIN, 'out');
          (function () {
            gpio.write(GPIO_PIN, 1);
          }).should.not.throw();
          gpio.destroy(done);
        });
      });
    });
  });
});
