#!/usr/bin/python

def main():
    while True:
        with open("/tmp/cardIDs.fifo", "r", 3) as fifo:
            line = fifo.readline().rstrip()
            print(line)


main()
