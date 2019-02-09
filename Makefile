all: invertedIndex.c
	gcc -Wall -Werror -o invertedIndex invertedIndex.c

clean:
	rm invertedIndex
