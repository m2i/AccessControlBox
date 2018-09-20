# AccessControlBox
The code for the access control boxes.

## Status (9/20/2018)
The code can read a card number from the reader as a keyboard device without using stdinput.
Card numbers are outputted to a fifo (`/tmp/cardIDs.fifo`) which is read by the authorizer python script.
The authorizer currently just prints the cards as they are read.
