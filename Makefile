all: readKbd.c
	gcc -o readKbd -lwiringPi readKbd.c
