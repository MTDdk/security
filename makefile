sha256 : Sha256.o
	gcc -o sha256 Sha256.o

Sha256.o : Sha256.c
	gcc -c Sha256.c
