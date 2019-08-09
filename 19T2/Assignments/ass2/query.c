// query.h, query.c ... an ADT for query scanners
// query.c ... query scan functions
// part of Multi-attribute Linear-hashed Files
// Manage creating and using Query objects
// Last modified by John Shepherd, July 2019

#include "defs.h"
#include "query.h"
#include "reln.h"
#include "tuple.h"

// Add hash functions
#include "hash.h"

// A suggestion ... you can change however you like

struct QueryRep {
	Reln    rel;       // need to remember Relation info
	Bits    known;     // the hash value from MAH
	Bits    unknown;   // the unknown bits from MAH
	PageID  curpage;   // current page in scan
	int     is_ovflow; // are we in the overflow pages?
	Offset  curtup;    // offset of current tuple within page
	//TODO
	char 	*qstr;		// input query string

};


// check whether a query is valid for a relation
// e.g. same number of attributes

int checkQuery(Reln r, char *q)
{
	if (*q == '\0') return 0;
	char *c;
	int nattr = 1;
	for (c = q; *c != '\0'; c++)
		if (*c == ',') nattr++;
	return (nattr == nattrs(r));
}



// take a query string (e.g. "1234,?,abc,?")
// set up a QueryRep object for the scan

/*
// 参数 1： 关系（表）r
// 参数 2： 查询字符串 q
// 目的： 将 q 转为自定义个查询对象 QueryRep

Query startQuery(Reln r, char *q)
{
	// 步骤 1： 声明一个 QueryRep 类型对象
	Query new = malloc(sizeof(struct QueryRep));
	assert(new != NULL);
	// TODO
	// Partial algorithm:
	// form known bits from known attributes
	// form unknown bits from '?' attributes
	// compute PageID of first page
	//   using known bits and first "unknown" value
	// set all values in QueryRep object 

	// 步骤 2： 	判断查询字符串 q 是否符合当前的关系（表） r 
	if (!checkQuery(r,q)) return NULL;

	// 步骤 3： 设定 rel，qstr
	new->rel = r;
	new->qstr = q;

	// 步骤 4： 通过 tupleVals() 函数获取 q 中每个特征的字符串表达形式
	Count nvals = nattrs(r);	// return #attributes
	ChVecItem *cv = chvec(r);	// return choice vector 
	char **vals = malloc(nvals*sizeof(char *));		// declare an array of strings "vals"
	assert(vals != NULL);		// decide malloc successful or not
	tupleVals(q, vals);			// extract tuple and convert it into an array of strings (one string for each attr)

	// 步骤 5： 遍历每个特征，设定 known，unknown
	Bits hash[nvals];
	Count dep = depth(r);
	int i;
	for(i = 0; i < nvals; i++) {
		// 如果对应的特征不是 ? 的话，那么就计算特征值的 hash value
		// 并且根据 cv 产生指定位置的 known，unknown
		// 什么是known，unknown？
		if(strcmp(vals[i],"?") != 0) {		// Case-I: known attribute
			hash[i] = hash_any((unsigned char *)vals[i],strlen(vals[i]));	// get hash of known attribute
			int j;
			for(j = 0; j < dep; j++) {		// iterate each position in combined hash
				if(cv[j].att == i) {		// if current attribute is in the first dep cv
					if(bitIsSet(hash[i], cv[j].bit)) {	// if corresponding bit is 1
						new->known = setBit(new->known, j);
					}else {
						new->known = unsetBit(new->known, j);
					}
					new->unknown = unsetBit(new->unknown, j);
				}
			}
		}
		// 如果对应的特征刚好是 ? 的话，那么就根据 cv 产生指定位置的 known，unknown
		else {								// Case-II: unknown attribute
			int j;
			for(j = 0; j < dep; j++) {
				if(cv[j].att == i) {
					new->known = unsetBit(new->known, j);
					new->unknown = setBit(new->unknown, j);
				}
			}
		}
	}

	// 步骤 6： 设定 curpage，is_ovflow
	for(i=0; i < dep; i++) {
		if(bitIsSet(new->known,i)) {
			new->curpage = setBit(new->curpage, i);
		}else {
			new->curpage = unsetBit(new->curpage, i);
		}
	}

	Page pg = getPage(dataFile(r), new->curpage);
	new->is_ovflow = pageOvflow(pg);

	freeVals(vals, nvals);

	return new;	
}
*/


// q is "v1,v2,v3,v4,..."
Query startQuery(Reln r, char *q)
{
	Query new = malloc(sizeof(struct QueryRep));
	assert(new != NULL);
	// TODO 
	// Partial algorithm:
	// form known bits from known attributes
	// form unknown bits from '?' attributes
	// compute PageID of first page
	//   using known bits and first "unknown" value
	// set all values in QueryRep object 

	// Step-1: Compute how many attributes in parameter list.	
	if (!checkQuery(r,q)) return NULL;

	// Step-2: Compute rel, q, known & unknown.
	new->rel = r;
	new->qstr = q;

	Count nvals = nattrs(r);	// return #attributes
	ChVecItem *cv = chvec(r);	// return choice vector 
	char **vals = malloc(nvals*sizeof(char *));		// declare an array of strings "vals"
	assert(vals != NULL);		// decide malloc successful or not
	tupleVals(q, vals);			// extract tuple and convert it into an array of strings (one string for each attr)

	Bits hash[nvals];
	Count dep = depth(r);
	int i;
	for(i = 0; i < nvals; i++) {
		if(strcmp(vals[i],"?") != 0) {		// Case-I: known attribute
			hash[i] = hash_any((unsigned char *)vals[i],strlen(vals[i]));	// get hash of known attribute
			int j;
			for(j = 0; j < dep; j++) {		// iterate each position in combined hash
				if(cv[j].att == i) {		// if current attribute is in the first dep cv
					if(bitIsSet(hash[i], cv[j].bit)) {	// if corresponding bit is 1
						new->known = setBit(new->known, j);
					}else {
						new->known = unsetBit(new->known, j);
					}
					new->unknown = unsetBit(new->unknown, j);
				}
			}
		}else {								// Case-II: unknown attribute
			int j;
			for(j = 0; j < dep; j++) {
				if(cv[j].att == i) {
					new->known = unsetBit(new->known, j);
					new->unknown = setBit(new->unknown, j);
				}
			}
		}
	}

	// Step-3: Compute curpage.
	for(i=0; i < dep; i++) {
		if(bitIsSet(new->known,i)) {
			new->curpage = setBit(new->curpage, i);
		}else {
			new->curpage = unsetBit(new->curpage, i);
		}
	}

	free(vals);

	return new;
}


// get next tuple during a scan

Tuple getNextTuple(Query q)
{
	// TODO
	// Partial algorithm:
	// if (more tuples in current page)
	//    get next matching tuple from current page
	// else if (current page has overflow)
	//    move to overflow page
	//    grab first matching tuple from page
	// else
	//    move to "next" bucket
	//    grab first matching tuple from data page
	// endif
	// if (current page has no matching tuples)
	//    go to next page (try again)
	// endif 
	
	while(1) {
		// ****Case-I: more tuples in current page****
		// Step-1: Acquire current date page and tuples in this page of query.
		Page curPage = getPage(dataFile(q->rel), q->curpage);	// current page 
		q->is_ovflow = pageOvflow(curPage);			// offset of overflow page of current page
		Count ntups = pageNTuples(curPage);	// how many tuples in current page	
		char *startData;
		startData = pageData(curPage);				// tuples string, comma-separate

//		printf("There are %d tuples in current page, like %s \n", ntups, startData);

		// Step-2: Iterate each tuple in curpage.
		int i;
		for(i = 0; i < ntups; i++) {
			if(tupleMatch(q->rel, q->qstr, statOfData)) {			// match
				Tuple tuple = startData;							// return tuple
				startData = startData + tupLength(startData) + 1;	// move pointer
				return tuple;
			}else {													// not match
				startData = startData + tupLength(startData) + 1;	// move pointer
			}
		}

		// ****Case-II: current page has overflow****
		if(q->is_ovflow != NO_PAGE) {
			// Update curpage, is_ovflow.
			q->curpage = pageOvflow(curPage);		// move to overflow page
			continue ;
		}

		// ****Case-III: others****
		Count dep = depth(q->rel);
		q->curpage = q->curpage + (1<<dep);

		if(q->is_ovflow == NO_PAGE) break;

	}

	return NULL;
}

// clean up a QueryRep object and associated data

void closeQuery(Query q)
{
	// TODO
	free(q);
}
