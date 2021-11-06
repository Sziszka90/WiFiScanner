INC_DIR = include
SRC_DIR = src
INC_DIR_NL = /usr/include/libnl3

$(CC) = gcc

main:
	$(CC) $(SRC_DIR)/utils.c $(SRC_DIR)/main.c -I$(INC_DIR_NL) -lnl-genl-3 -lnl-3 -I$(INC_DIR) -o wifi_scanner

clean:
	rm -f core *.o main
