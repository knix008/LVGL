CC = gcc
CFLAGS = -Wall -Wextra -std=c99 $(shell pkg-config --cflags wayland-client)
LIBS = $(shell pkg-config --libs wayland-client)

PROGRAM = hello-world
SOURCES = hello-world.c xdg-shell-protocol.c

all: $(PROGRAM)

# Generate the XDG shell protocol files
xdg-shell-protocol.c: /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml
	wayland-scanner private-code < $< > $@

xdg-shell-client-protocol.h: /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml
	wayland-scanner client-header < $< > $@

$(PROGRAM): hello-world.c xdg-shell-protocol.c xdg-shell-client-protocol.h
	$(CC) $(CFLAGS) -o $(PROGRAM) hello-world.c xdg-shell-protocol.c $(LIBS)

clean:
	rm -f $(PROGRAM) xdg-shell-protocol.c xdg-shell-client-protocol.h

install:
	cp $(PROGRAM) /usr/local/bin/

.PHONY: all clean install