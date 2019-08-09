// tuple.h, tuple.c ... an ADT for tuples
// tuple.c ... functions on tuples
// part of Multi-attribute Linear-hashed Files
// Last modified by John Shepherd, July 2019

#include "defs.h"
#include "tuple.h"
#include "reln.h"
#include "hash.h"
#include "chvec.h"
#include "bits.h"

// return number of bytes/chars in a tuple

int tupLength(Tuple t)
{
	return strlen(t);
}

// reads/parses next tuple in input

Tuple readTuple(Reln r, FILE *in)
{
	char line[MAXTUPLEN];
	if (fgets(line, MAXTUPLEN-1, in) == NULL)
		return NULL;
	line[strlen(line)-1] = '\0';
	// count fields
	// cheap'n'nasty parsing
	char *c; int nf = 1;
	for (c = line; *c != '\0'; c++)
		if (*c == ',') nf++;
	// invalid tuple
	if (nf != nattrs(r)) return NULL;
	return copyString(line); // needs to be free'd sometime
}

// extract values into an array of strings

void tupleVals(Tuple t, char **vals)
{
	char *c = t, *c0 = t;
	int i = 0;
	for (;;) {
		while (*c != ',' && *c != '\0') c++;
		if (*c == '\0') {
			// end of tuple; add last field to vals
			vals[i++] = copyString(c0);
			break;
		}
		else {
			// end of next field; add to vals
			*c = '\0';
			vals[i++] = copyString(c0);
			*c = ',';
			c++; c0 = c;
		}
	}
}

// release memory used for separate attirubte values

void freeVals(char **vals, int nattrs)
{
	int i;
	for (i = 0; i < nattrs; i++) free(vals[i]);
}



// hash a tuple using the choice vector
// TODO: actually use the choice vector to make the hash
// Modify the following function to use the relevant bits from each attribute hash value to form a composite hash

/*
// 参数1：关系（表）r
// 参数2：元组 t
// 目的：利用关系 r 的 cv 属性，提取元组 t 中每个特征的对应 bit 组合成 composite hash 

Bits tupleHash(Reln r, Tuple t)
{
	// 步骤 1： 通过 tupleVals() 函数获取 t 中每个特征的字符串表达形式
	char buf[MAXBITS+1];
	Count nvals = nattrs(r);	// return #attributes
	char **vals = malloc(nvals*sizeof(char *));		// declare an array of strings "vals"
	assert(vals != NULL);		// decide malloc successful or not
	tupleVals(t, vals);			// extract tuple and convert it into an array of strings (one string for each attr)

	// 步骤 2： 通过字符串计算每个特征的 hash value
	Bits *h = malloc(nvals*sizeof(Bits));
	int i;
	for(i=0; i < nvals; i++) {
		h[i] = hash_any((unsigned char *)vals[i],strlen(vals[i]));
	}

	// 步骤 3： 根据 r.cv 计算 composite hash（因为题目要求 hash 值是 32bits，所以循环 MAXCHVEC 次）
	ChVecItem *cv = chvec(r);	// return a set of cv 
	Bits res = 0, oneBit;		// define return composite hash
	int a, b;					// define a & b to store cv.attr & cv.bit
	for (i = 0; i < MAXCHVEC; i++) {	
		// 获得第 i 位对应的 cv 信息
		a = cv[i].attr;
		b = cv[i].bit;
		// 从特征 a 的 hash 中提取第 b 个 位置的 bit
		oneBit = bitIsSet(h[a],b);
		// 将提取的 bit 插入到 composite hash 的第 i 个位置
		// res = res | (oneBit << i);
		res = (oneBit == 1) ? setBit(res, i) : unsetBit(res, i);
	}

	// 步骤 4： 释放内存，并打印测试
	free(h);
	freeVals(vals, nvals);

	bitsString(res, buf);
	printf("hash = %s\n", buf);

	return res;
}
*/

Bits tupleHash(Reln r, Tuple t)
{
	char buf[MAXBITS+1];

	Count nvals = nattrs(r);	// return #attributes

	char **vals = malloc(nvals*sizeof(char *));		// declare an array of strings "vals"

	assert(vals != NULL);		// decide malloc successful or not

	tupleVals(t, vals);			// extract tuple and convert it into an array of strings (one string for each attr)

	// **************Through choice vector (r->cv) to built combined hash (32)**************
	// Step-1: Generate hash of each attribute.
	Bits *hashSet = malloc(nvals*sizeof(Bits));
	int i;
	for(i=0; i < nvals; i++) {
		hashSet[i] = hash_any((unsigned char *)vals[i],strlen(vals[i]));
		bitsString(hashSet[i],buf);
	}

	// Step-2: Acquire choise vector from Reln, and compute the composite hash string. 	
	ChVecItem *cvItems = chvec(r);					// return a set of cv 
	char *combined = malloc(MAXBITS*sizeof(char));	// define string "combined" to store composite hash

	for (i = 0; i < MAXCHVEC; i++) {	
		bitsString(hashSet[cvItems[i].att],buf);
		// Case-I: bit in range [0,7], MAXBITS+2; Case-II: bit in range [8,15], MAXBITS+1
		// Case-III: bit in range [16,23], MAXBITS; Case-II: bit in range [24,31], MAXBITS-1 
		combined[MAXBITS-1-i] = buf[MAXBITS+2-cvItems[i].bit/8-cvItems[i].bit];
	}

	// Step-3: Convert composite hash string into Bits type hash, and return it.
	int multiplier = 0;
	Bits hash;
	for(i=MAXBITS-1; i>=0; i--) {
		hash += (combined[i]*(1<<multiplier++));
	}

	bitsString(hash,buf);		// convert hash bits to string for printing
	printf("hash = %s\n", buf);

	free(vals);
	return hash;
}


// compare two tuples (allowing for "unknown" values)

Bool tupleMatch(Reln r, Tuple t1, Tuple t2)
{
	Count na = nattrs(r);
	char **v1 = malloc(na*sizeof(char *));
	tupleVals(t1, v1);
	char **v2 = malloc(na*sizeof(char *));
	tupleVals(t2, v2);
	Bool match = TRUE;
	int i;
	for (i = 0; i < na; i++) {
		// assumes no real attribute values start with '?'
		if (v1[i][0] == '?' || v2[i][0] == '?') continue;
		if (strcmp(v1[i],v2[i]) == 0) continue;
		match = FALSE;
	}
	freeVals(v1,na); freeVals(v2,na);
	return match;
}

// puts printable version of tuple in user-supplied buffer

void tupleString(Tuple t, char *buf)
{
	strcpy(buf,t);
}
