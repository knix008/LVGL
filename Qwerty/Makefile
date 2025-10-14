# Makefile for Korean/English QWERTY Keypad

CC = gcc
CFLAGS = -Wall -Wextra -std=c99
GTK_CFLAGS = $(shell pkg-config --cflags gtk+-3.0)
GTK_LIBS = $(shell pkg-config --libs gtk+-3.0)

TARGET = qwerty
OBJS = main.o qwerty.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(GTK_CFLAGS) -o $(TARGET) $(OBJS) $(GTK_LIBS)

main.o: main.c qwerty.h
	$(CC) $(CFLAGS) $(GTK_CFLAGS) -c main.c

qwerty.o: qwerty.c qwerty.h
	$(CC) $(CFLAGS) -c qwerty.c

clean:
	rm -f $(TARGET) $(OBJS)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
