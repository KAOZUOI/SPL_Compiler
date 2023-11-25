CC=gcc
FLEX=flex
BISON=bison

CFLAGS = -Iinclude -Wall

DEBUG ?= 0
VERBOSE ?= 0

# # test
build: src/syntax.y src/lexical.l src/APT.c include/APT.h
	$(BISON) -t -d src/syntax.y
	$(FLEX) src/lexical.l
	$(CC) syntax.tab.c src/APT.c -o splc -ly -lfl $(CFLAGS) 
	rm -f *.yy.* *.tab.*
clean:
	@rm -f *.yy.* *.tab.* splc
.PHONY: clean

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
