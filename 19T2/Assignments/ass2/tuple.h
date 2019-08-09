// tuple.h ... interface to functions on Tuples
// part of Multi-attribute Linear-hashed Files
// A Tuple is just a '\0'-terminated C string
// Consists of "val_1,val_2,val_3,...,val_n"
// See tuple.c for details on functions
// Last modified by John Shepherd, July 2019

#ifndef TUPLE_H
#define TUPLE_H 1

typedef char *Tuple;

#include "reln.h"
#include "bits.h"

int tupLength(Tuple t);
Tuple readTuple(Reln r, FILE *in);
Bits tupleHash(Reln r, Tuple t);
void tupleVals(Tuple t, char **vals);
void freeVals(char **vals, int nattrs);
Bool tupleMatch(Reln r, Tuple t1, Tuple t2);
void tupleString(Tuple t, char *buf);

#endif
