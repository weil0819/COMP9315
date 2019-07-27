// create.c ... a main program that creates a new MALH relation
// create.c ... create an empty Relation
// part of Multi-attribute linear-hashed files
// Ask a query on a named file
// Usage:  ./create  [-v]  RelName  #attrs  #pages  ChoiceVector
// where #attrs = # of attributes in each tuple
//	   #pages = initial (empty) pages in File
//	   ChoiceVector = attr,bit:attr,bit:...

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "util.h"
#include "reln.h"

#define USAGE "./create  [-v]  RelName  #attrs  #pages  ChoiceVector"


// Main ... process args, create relation

int main(int argc, char **argv)
{
	//Reln r;  // handle on the data file
	int nattrs;  // number of attributes in each tuple
	int npages;  // initial number of pages
	char err[MAXERRMSG];  // buffer for error messages
	int verbose;  // show extra info on query progress
	char *rname;  // name of table/file
	char *attrs;   // number of attributes in tuples
	char *pages;   // number of pages in data file
	char *cv;	  // choice vector

	// Process command-line args

	if (argc < 2) fatal(USAGE);
	if (strcmp(argv[1], "-v") == 0) {
		if (argc < 6) fatal(USAGE);
	    verbose = 1; rname = argv[2]; attrs = argv[3]; pages = argv[4]; cv = argv[5];
	}
	else {
		if (argc < 5) fatal(USAGE);
	    verbose = 0; rname = argv[1]; attrs = argv[2]; pages = argv[3]; cv = argv[4];
	}

	// how many attributes in each tuple
	nattrs = atoi(attrs);
	if (nattrs < 2 || nattrs > 10) {
		sprintf(err, "Invalid #attrs: %d (must be 1 < # < 11)", nattrs);
		fatal(err);
	}

	// how many initally empty pages
	npages = atoi(pages);
	if (npages < 1 || npages > 64) {
		sprintf(err, "Invalid #pages: %d (must be 0 < # < 65)", nattrs);
		fatal(err);
	}
	// convert to least 2^d >= npages
	// d gives initial depth of file
	int d = 0, np = 1;
	while (np < npages) { d++; np <<= 1; }

	if (verbose)
		printf("#a=%d, #p=%d, d=%d\n", nattrs, np, d);

	// Open files for the Relation and initialise

	if (existsRelation(rname)) {
		sprintf(err, "Relation %s already exists", rname);
		fatal(err);
	}
	if (newRelation(rname, nattrs, np, d, cv) != OK) {
		sprintf(err, "Problems while creating relation %s", rname);
		fatal(err);
	}
	return OK;
}
