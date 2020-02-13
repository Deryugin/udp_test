CC=gcc
DEPS = hellomake.h

all: client.c server.c
	$(CC) -o client client.c 
	$(CC) -o server server.c 
