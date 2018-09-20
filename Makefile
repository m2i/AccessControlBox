bin = ./bin

all: Folders CardReader Authorizer
	@

Folders:
	mkdir -p $(bin)

CardReader: readKbd.c
	gcc -o $(bin)/readKbd readKbd.c -ggdb

Authorizer: authorizer.py
	cp authorizer.py $(bin)/authorizer
	chmod +x $(bin)/authorizer

clean:
	rm $(bin)/*
