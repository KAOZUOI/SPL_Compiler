# CC=gcc
# FLEX=flex
# BISON=bison

# CFLAGS = -Iinclude -Wall

# DEBUG ?= 0
# VERBOSE ?= 0

# # test -v
# build: src/syntax.y src/lexical.l src/APT.c include/APT.h src/translate.c include/translate.h src/symbolTable.c include/symbolTable.h include/type.h
# 	@mkdir -p bin/
# 	$(BISON) -t -d src/syntax.y
# 	$(FLEX) src/lexical.l
# 	$(CC) syntax.tab.c src/APT.c src/symbolTable.c src/translate.c -o bin/splc -lfl -g -D DEBUG $(CFLAGS) 
# 	rm -f *.yy.* *.tab.*
# clean:
# 	@rm -rf bin/
# 	@rm -f *.yy.* *.tab.* splc	
# .PHONY: clean

CC=gcc
BIN=bin
SRC=src/main.c src/tacReader.c src/mips32.c

splc: clean
	@mkdir -p $(BIN)
	$(CC) $(SRC) -o $(BIN)/$@
clean:
	@rm -rf $(BIN)
.PHONY: clean

# # test IR generate
# build: src/syntax.y src/lexical.l src/APT.c include/APT.h src/translate.c include/translate.h src/symbolTable.c include/symbolTable.h include/type.h src/type.c include/tac.h src/tac.c include/hashmap.h src/hashmap.c
# 	@mkdir -p bin/
# 	$(BISON) -t -d src/syntax.y
# 	$(FLEX) src/lexical.l
# 	$(CC) syntax.tab.c src/APT.c src/symbolTable.c src/translate.c src/tac.c src/hashmap.c src/type.c src/optimize.c -o bin/splc -lfl -g $(CFLAGS) 
# 	rm -f *.yy.* *.tab.*
# clean:
# 	@rm -rf bin/
# 	@rm -f *.yy.* *.tab.* splc	
# .PHONY: clean


# # test -v
# build: src/syntax.y src/lexical.l src/APT.c include/APT.h
# 	$(BISON) -t -d -v src/syntax.y
# 	$(FLEX) src/lexical.l
# 	$(CC) syntax.tab.c src/APT.c -o splc -ly -lfl $(CFLAGS)
# 	rm -f *.yy.* *.tab.*
# clean:
# 	@rm -f *.yy.* *.tab.* splc
# .PHONY: clean

# # debug flex
# build: src/syntax.y src/lexical_debug.l src/APT.c include/APT.h
# 	$(BISON) -t -d src/syntax.y
# 	$(FLEX) src/lexical_debug.l
# 	$(CC) syntax.tab.c src/APT.c -o splc -ly -lfl $(CFLAGS)
# 	rm -f *.yy.* *.tab.*
# clean:
# 	@rm -f *.yy.* *.tab.* splc
# .PHONY: clean

# debug bison
# build: src/syntax_debug.y src/lexical_debug.l src/APT.c include/APT.h
# 	$(BISON) -t -d src/syntax_debug.y
# 	$(FLEX) src/lexical_debug.l
# 	$(CC) syntax_debug.tab.c src/APT.c -o splc -ly -lfl $(CFLAGS)
# 	rm -f *.yy.* *.tab.*
# clean:
# 	@rm -f *.yy.* *.tab.* splc
# .PHONY: clean
