CC=gcc
CFLAGS=-I${NGX_PATH}/src/os/unix -I${NGX_PATH}/src/core -I${NGX_PATH}/src/http -I${NGX_PATH}/src/http/modules -I${NGX_PATH}/src/event -I${NGX_PATH}/objs/ -I.


all:

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: all clean test nginx


NGX_OBJS := $(shell find ${NGX_PATH}/objs -name \*.o)

nginx:
	cd ${NGX_PATH} && rm -rf ${NGX_PATH}/objs/src/core/nginx.o && make

test: | nginx
	strip -N main -o ${NGX_PATH}/objs/src/core/nginx_without_main.o ${NGX_PATH}/objs/src/core/nginx.o
	mv ${NGX_PATH}/objs/src/core/nginx_without_main.o ${NGX_PATH}/objs/src/core/nginx.o
	$(CC) test_suite.c $(CFLAGS) -o test_suite -lcmocka ${NGX_OBJS} -ldl -lpthread -lcrypt -lpcre -lcrypto -lcrypto -lz $<
	./test_suite

clean:
	rm -f *.o test_suite

# vim: ft=make ts=8 sw=8 noet	
