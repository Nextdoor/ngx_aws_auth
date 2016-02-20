CC=gcc
CFLAGS=-I${NGX_PATH}src/os/unix -I${NGX_PATH}src/core -I${NGX_PATH}src/http -I${NGX_PATH}src/http/modules -I${NGX_PATH}src/event -I${NGX_PATH}objs/ 


all: aws_functions.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: all clean

clean:
	rm -f *.o 
