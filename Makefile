CC = gcc
CFLAGS = -Wall -Wextra -Werror -Wshadow -Wdouble-promotion -fno-common -Wconversion  -fsanitize=address
OBJFILES = server.o parse_request.o
DEPS = parse_request.h
TARGET = server

all: $(TARGET)

# $(TARGET): $(OBJFILES)
# @$(CC) $(CFLAGS) -g $(TARGET).c -o ./bin/$(TARGET) $(OBJFILES)
#$(TARGET): $(OBJFILES)
#	$(CC) $(CFLAGS) -o ./bin/$(TARGET) $(OBJFILES)
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJFILES)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm *.o