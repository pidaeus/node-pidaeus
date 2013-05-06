describe('GPIO', function () {
  var GPIO = pidaeus.GPIO;

  describe('getters', function () {
    it('.INPUT', function () {
      GPIO.should.have.property('INPUT');
    });

    it('.OUTPUT', function () {
      GPIO.should.have.property('OUTPUT');
    });
  });
});
