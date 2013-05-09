
# Normal Build Variables
BUILDS = debug release
OUTDIR ?= out
GYPFILES = libpi.gyp

# Gyp
GYPDIR = support/gyp

#
# Normal Builds
#

all: release

$(BUILDS): $(GYPDIR) $(OUTDIR)/Makefile
	@$(MAKE) \
		-C "$(OUTDIR)" \
		-f Makefile \
		builddir="$(shell pwd)/$(OUTDIR)/$@" \
		BUILDTYPE=$(shell echo $@ | python -c "print raw_input().capitalize()")

$(OUTDIR)/Makefile: $(GYPFILES)
	@GYP_GENERATORS=make $(GYPDIR)/gyp \
		--generator-output="$(OUTDIR)" \
		--depth . \
		$(GYPFILES)

#
# Test
#

test: release
	@./$(OUTDIR)/release/test

#
# Dependencies
#

$(GYPDIR): 
	git clone https://git.chromium.org/external/gyp.git $(GYPDIR)

#
# Cleaning up
#

clean: clean-out

clean-out:
	rm -rf $(OUTDIR)

clean-all: clean
	rm -rf $(GYPDIR)

#
# etc ... 
# 

.PHONY: $(Builds)
.PHONY: all 
.PHONY: clean clean-out clean-all
.PHONY: test
