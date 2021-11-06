INC_DIR = include
SRC_DIR = src
INC_DIR_NL = /usr/include/libnl3

$(CC) = gcc

main:
	$(CC) $(SRC_DIR)/utils.c $(SRC_DIR)/main.c -I$(INC_DIR) -I$(INC_DIR_NL) -lnl-genl-3 -lnl-3 -o main

clean:
	rm -f core *.o main

gcc utils.c main.c -I/usr/include/libnl3 -lnl-genl-3 -lnl-3 -I../include -o util
