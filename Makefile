CFLAGS = -Wall -Werror -std=gnu11 -O2


compile:
	gcc ${CFLAGS} main.c -o output

asan:
	gcc main.c -o output -fsanitize=address
