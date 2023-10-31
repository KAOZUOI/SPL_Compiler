CC=gcc
FLEX=flex
BISON=bison
SRC_DIR=./src

ip:
	$(BISON) -d $(SRC_DIR)/syntax.y
	$(FLEX) $(SRC_DIR)/lex.l
	$(CC) $(SRC_DIR)/syntax.tab.c -lfl -ly -o test.out

clean:
	@rm -f $(SRC_DIR)/lex.yy.c $(SRC_DIR)/syntax.tab.c $(SRC_DIR)/syntax.tab.h *.out

.PHONY: ip clean

