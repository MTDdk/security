sha256 : Sha256.o
	gcc -o sha256 Sha256.o
geo : GeoHash.o
	gcc -o geo GeoHash.o

Sha256.o : Sha256.c
	gcc -c Sha256.c

GeoHash.o : GeoHash.c
	gcc -c GeoHash.c
