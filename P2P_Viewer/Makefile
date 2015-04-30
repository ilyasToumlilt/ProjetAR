CC=gcc 
CFLAGS=-Wall -ansi

all: bin/p2p_viewer

bin/p2p_viewer: obj/p2p_viewer.o
	$(CC) -o $@ $^ $(LDFLAGS)

obj/p2p_viewer.o: p2p_viewer.c
	$(CC)  -o $@ -c $^ $(CFLAGS)

clean: 
	rm -rf obj/p2p_viewer.o bin/p2p_viewer
