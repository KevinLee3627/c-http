CC = gcc
CFLAGS = -Wall -Wextra -Werror -Wshadow -Wdouble-promotion -fno-common -Wconversion  -fsanitize=address
TARGET = server

all: $(TARGET)

$(TARGET): $(TARGET).c
	@$(CC) $(CFLAGS) -g server.c -o ./bin/server

clean:
	rm *.o