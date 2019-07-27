// reln.h ... interface to functions on Relations
// part of Multi-attribute Linear-hashed Files
// See reln.c for details on Reln type and functions
// Last modified by John Shepherd, July 2019

#ifndef RELN_H
#define RELN_H 1

typedef struct RelnRep *Reln;

#include "defs.h"
#include "tuple.h"
#include "page.h"
#include "chvec.h"

Status newRelation(char *name, Count nattr, Count npages, Count d, char *cv);
Reln openRelation(char *name, char *mode);
void closeRelation(Reln r);
Bool existsRelation(char *name);
PageID addToRelation(Reln r, Tuple t);
FILE *dataFile(Reln r);
FILE *ovflowFile(Reln r);
Count nattrs(Reln r);
Count npages(Reln r);
Count depth(Reln r);
Count splitp(Reln r);
ChVecItem *chvec(Reln r);
void relationStats(Reln r);

#endif
