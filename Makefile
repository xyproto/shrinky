PREFIX ?= /usr
PYTHON ?= python3
SITE_PACKAGES := $(shell $(PYTHON) -c "import sysconfig; print(sysconfig.get_paths()['purelib'])")

.PHONY: install uninstall

all:
	@echo 'Nothing to build.'

install:
	install -d "$(DESTDIR)$(SITE_PACKAGES)/shrinky"
	install -m 644 shrinky/*.py "$(DESTDIR)$(SITE_PACKAGES)/shrinky"
	install -Dm 755 shrinky.sh "$(DESTDIR)$(PREFIX)/bin/shrinky"

uninstall:
	rm -rf "$(DESTDIR)$(SITE_PACKAGES)/shrinky"
	rm -f "$(DESTDIR)$(PREFIX)/bin/shrinky"
