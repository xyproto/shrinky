.PHONY: all clean install uninstall test

PREFIX ?= /usr
PYTHON ?= python3
SITE_PACKAGES := $(shell $(PYTHON) -c "import sysconfig; print(sysconfig.get_paths()['purelib'])")

all:
	@echo 'Nothing to build.'

clean:
	@echo 'Nothing to clean.'

install:
	install -d "$(DESTDIR)$(SITE_PACKAGES)/shrinky"
	install -m 644 shrinky/*.py "$(DESTDIR)$(SITE_PACKAGES)/shrinky"
	install -Dm 755 shrinky.sh "$(DESTDIR)$(PREFIX)/bin/shrinky"

uninstall:
	rm -rf "$(DESTDIR)$(SITE_PACKAGES)/shrinky"
	rm -f "$(DESTDIR)$(PREFIX)/bin/shrinky"

test:
	@echo "=== GLSL minification ==="
	$(PYTHON) -m shrinky examples/quad_430.frag.glsl
	$(PYTHON) -m shrinky examples/quad_430.vert.glsl
	@echo "=== Full compilation ==="
	@touch examples/shrinky.h
	$(PYTHON) -m shrinky -v examples/intro.cpp
	@rm -f examples/shrinky.h examples/intro examples/intro.bin examples/intro.ld
	@rm -f examples/intro.unprocessed examples/intro.stripped examples/intro.o
	@rm -f examples/intro.S examples/intro.final.S examples/intro.final.o
	@rm -f examples/*.preprocessed
	@echo "=== All tests passed ==="
