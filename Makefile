CC=gcc -g
CFLAGS=-I.

client: client.c requests.c helpers.c buffer.c
	$(CC) -o client client.c requests.c helpers.c buffer.c parson.c -Wall

run: client
	./client

val: client
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./client

clean:
	rm -f *.o client
