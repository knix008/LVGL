CC = gcc
WAYLAND_CFLAGS = $(shell pkg-config --cflags wayland-client)
WAYLAND_LIBS = $(shell pkg-config --libs wayland-client)

# Protocol files
PROTOCOL_HEADER = xdg-shell-client-protocol.h
PROTOCOL_IMPL = xdg-shell-client-protocol.c

# Targets
WESTON_TARGET = helloworld

all: $(WESTON_TARGET)

$(WESTON_TARGET): helloworld.c $(PROTOCOL_HEADER) $(PROTOCOL_IMPL)
	$(CC) $(WAYLAND_CFLAGS) -o $(WESTON_TARGET) helloworld.c $(PROTOCOL_IMPL) $(WAYLAND_LIBS) -lm

$(PROTOCOL_HEADER): xdg-shell.xml
	wayland-scanner client-header $< $@

$(PROTOCOL_IMPL): xdg-shell.xml
	wayland-scanner private-code $< $@

clean:
	rm -f $(WESTON_TARGET) $(PROTOCOL_HEADER) $(PROTOCOL_IMPL)

.PHONY: all clean
