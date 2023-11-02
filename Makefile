CC=gcc
FLEX=flex
BISON=bison

DEBUG ?= 0
VERBOSE ?= 0
ifeq ($(DEBUG), 1)
    CFLAGS:=-DDEBUG -g $(CFLAGS)
endif

ifeq ($(VERBOSE), 1)
    CFLAGS:=-DVERBOSE $(CFLAGS)
endif

build: src/syntax.y src/lexical.l src/APT.c src/APT.h
	$(BISON) -t -d src/syntax.y
	$(FLEX) src/lexical.l
	$(CC) syntax.tab.c src/APT.c -o splc -ly -lfl $(CFLAGS)
	rm -f *.yy.* *.tab.*
clean:
	@rm -f *.yy.* *.tab.* splc
.PHONY: clean
