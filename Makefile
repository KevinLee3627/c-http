CC = gcc
CFLAGS = -Wall -Wextra -Werror -Wshadow -Wdouble-promotion -fno-common -Wconversion  -fsanitize=address
TARGET = server

$(TARGET): whatever.o
	@$(CC) $(CFLAGS) -g server.c -o ./bin/server

clean:
	rm *.o