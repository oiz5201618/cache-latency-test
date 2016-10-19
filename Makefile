CC = gcc
CFLAGS = -O0 -std=gnu99 -Wall 

default:
	$(CC) $(CFLAGS) main.c -lm -o cach_test
clean:
	rm cach_test
