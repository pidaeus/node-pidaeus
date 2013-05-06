describe('GPIO', function () {
  var GPIO = pidaeus.GPIO;

  it('(construct/deconstruct)', function () {
    var gpio = new GPIO;
    (function () {
      gpio.setup();
      gpio.teardown();
    }).should.not.throw();
  });
});
