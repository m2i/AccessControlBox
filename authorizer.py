#!/usr/bin/python

def main():
    while True:
        with open("/tmp/cardIDs.fifo", "r", 3) as fifo:
            while True:
                line = fifo.readline().rstrip()
                if (len(line)>0):
                    print(line)
                else:
                    fifo.close()
                    break
                


main()
