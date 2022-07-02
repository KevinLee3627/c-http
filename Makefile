CC = gcc
CFLAGS = -Wall -Wextra -Werror -Wshadow -Wdouble-promotion -fno-common -Wconversion  -fsanitize=address
TARGET = server

$(TARGET): whatever.o
	@$(CC) $(CFLAGS) -g whatever.o -o ./bin/server

$(TARGET).o: server.c
	gcc -c server.c -o whatever.o

clean:
	rm *.o