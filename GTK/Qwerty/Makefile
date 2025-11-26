# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# GTK3 flags using pkg-config
GTK_CFLAGS = $(shell pkg-config --cflags gtk+-3.0)
GTK_LIBS = $(shell pkg-config --libs gtk+-3.0)

# Source and object files
SRCS = main.c qwerty.c
OBJS = $(SRCS:.c=.o)

# Executable name
TARGET = qwerty

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(GTK_LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(GTK_CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)