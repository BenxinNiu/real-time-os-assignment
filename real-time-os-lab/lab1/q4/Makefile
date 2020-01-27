CC=	clang
CFLAGS=	-Weverything -g

all: syscalls
clean:
	rm -f syscalls *.o

syscalls: main.o stuff.o
	$(CC) main.o stuff.o -o syscalls