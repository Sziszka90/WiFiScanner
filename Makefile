INC_DIR = include
SRC_DIR = src
INC_DIR_NL = /usr/include/libnl3
CC = g++

main:
	$(CC) $(SRC_DIR)/wifi_lib.c $(SRC_DIR)/main.cpp -I$(INC_DIR_NL) -lnl-genl-3 -lnl-3 -I$(INC_DIR) -o main

clean:
	rm -f core *.o main
