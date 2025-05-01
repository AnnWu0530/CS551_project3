CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = 

# Source files
SRCS = main.c \
       parser.c \
       executor.c \
       signals.c \
       jobs.c \
       history.c \
       env.c \
       builtins.c \
       redirection.c \
       completion.c \
       config.c \
       glob.c \
       alias.c \
       error.c

# Object files
OBJS = $(SRCS:.c=.o)

# Header files
HEADERS = $(wildcard *.h)

# Executable
TARGET = mysh

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Compile source files into object files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)

# Create tests directory
tests:
	mkdir -p tests
	[ -f tests/test_basic.sh ] || cp test_basic.sh tests/
	chmod +x tests/test_basic.sh

# Run tests
test: $(TARGET) tests
	cd tests && ./test_basic.sh

# Install (optional)
install: $(TARGET)
	mkdir -p $(DESTDIR)/usr/local/bin
	cp $(TARGET) $(DESTDIR)/usr/local/bin/
	chmod 755 $(DESTDIR)/usr/local/bin/$(TARGET)

# Uninstall (optional)
uninstall:
	rm -f $(DESTDIR)/usr/local/bin/$(TARGET)

.PHONY: all clean test install uninstall tests