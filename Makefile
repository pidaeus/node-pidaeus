
LIBPI_V = "0.1.0"

TESTS = test/*.js
REPORTER = spec

test: build/Release/pidaeus.node 
	@NODE_ENV=test ./node_modules/.bin/mocha \
		--require ./test/bootstrap \
		--reporter $(REPORTER) \
		$(TESTS)

test-cov: lib-cov
	@pidaeus_COV=1 $(MAKE) test REPORTER=html-cov > coverage.html

lib-cov: clean
	@jscoverage lib lib-cov

build/Release/pidaeus.node: src/* binding.gyp
	@node-gyp rebuild

clean: clean-build clean-cov

clean-build:
	@rm -rf build

clean-cov:
	@rm -rf lib-cov
	@rm -f coverage.html

deps: deps/libpi

deps/libpi:
	@rm -rf deps/libpi
	@mkdir -p deps/libpi
	@curl -L https://github.com/pidaeus/libpi/archive/$(LIBPI_V).tar.gz \
		| tar -zx -C deps/libpi --strip 1

.PHONY: test lib-cov test-cov 
.PHONY: clean clean-build clean-cov clean
