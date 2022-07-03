CC = gcc
CFLAGS = -Wall -Wextra -Werror -Wshadow -Wdouble-promotion -fno-common -Wconversion -fsanitize=address -fno-omit-frame-pointer

SRCDIR = src
OBJDIR = $(SRCDIR)/obj

_DEPS = parse_request.h
DEPS = $(patsubst %,$(SRCDIR)/%,$(_DEPS))

_OBJFILES = server.o parse_request.o send_response.o
OBJFILES = $(patsubst %,$(OBJDIR)/%,$(_OBJFILES))

TARGETDIR = bin
TARGET = server

all: $(TARGET)

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJFILES)
	$(CC) -o $(TARGETDIR)/$@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm $(OBJDIR)/*.o

# Thank you to...
#	https://cs.colby.edu/maxwell/courses/tutorials/maketutor/
# https://stackoverflow.com/questions/54854128/use-of-o-c-in-makefile