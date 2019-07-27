// chvec.h ... interface to functions on ChoiceVectors
// part of Multi-attribute Linear-hashed Files
// A ChVec is an array of MAXCHVEC ChVecItems
// Each ChVecItem is a pair (attr#,bit#)
// See chvec.c for details on functions
// Last modified by John Shepherd, July 2019

#ifndef CHVEC_H
#define CHVEC_H 1

#include "defs.h"
#include "reln.h"

#define MAXCHVEC 32

typedef struct _ChVecItem { Byte att; Byte bit; } ChVecItem;

typedef ChVecItem ChVec[MAXCHVEC];

Status parseChVec(Reln r, char *str, ChVec cv);
void printChVec(ChVec cv);

#endif
