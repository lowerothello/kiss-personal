PREFIX=/usr/local

all:
	gcc lv2ports.c $(shell pkg-config --libs lilv-0)$(shell pkg-config --cflags lilv-0) -o lv2ports
clean:
	rm -f lv2ports
install:
	mkdir -p "$(DESTDIR)$(PREFIX)/bin"
	cp lv2ports "$(DESTDIR)$(PREFIX)/bin/lv2ports"
uninstall:
	rm -f "$(DESTDIR)$(PREFIX)/bin/lv2ports"
