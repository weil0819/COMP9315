# Makefile for COMP9315 16s1 Assignment 2
#
# Note:
# - there are no dependencies on *.h files
# - these define interfaces, and interfaces don't change

CC=gcc
CFLAGS=-Wall -Werror -g -std=c99
LIBS=query.o page.o reln.o tuple.o util.o chvec.o hash.o bits.o
BINS=create dump insert select stats gendata

all : $(BINS)

create: create.o $(LIBS)
dump: dump.o $(LIBS)
insert: insert.o $(LIBS)
select: select.o $(LIBS)
stats:  stats.o $(LIBS)
gendata: gendata.o $(LIBS)

create.o: create.c defs.h
dump.o: dump.c defs.h reln.h page.h
insert.o: insert.c defs.h reln.h tuple.h
select.o: select.c defs.h query.h tuple.h reln.h chvec.h hash.h bits.h
stats.o: stats.c defs.h reln.h
gendata.o: gendata.c defs.h

bits.o: bits.c bits.h
chvec.o: chvec.c defs.h chvec.h reln.h
hash.o: hash.c defs.h hash.h bits.h
page.o: page.c defs.h bits.h
query.o: query.c defs.h query.h reln.h tuple.h
reln.o: reln.c defs.h reln.h page.h tuple.h chvec.h hash.h bits.h
tuple.o: tuple.c defs.h tuple.h reln.h chvec.h hash.h bits.h
util.o: util.c

defs.h: util.h

db:
	rm -f R.*
	./create R 3 5 ""
	./gendata 1000 3 1234 | ./insert R

clean:
	rm -f $(BINS) *.o
