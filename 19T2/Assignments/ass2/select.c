// select.c ... a main program that finds tuples matching a PMR query
// select.c ... run queries
// part of Multi-attribute linear-hashed files
// Ask a query on a named relation
// Usage:  ./select  [-v]  RelName  v1,v2,v3,v4,...
// where any of the vi's can be "?" (unknown)

#include "defs.h"
#include "query.h"
#include "tuple.h"
#include "reln.h"
#include "chvec.h"

#define USAGE "./select  [-v]  RelName  v1,v2,v3,v4,..."

// Main ... process args, run query

int main(int argc, char **argv)
{
	Reln r;  // handle on the open relation
	Query q;  // processed version of query string
	Tuple t;  // tuple pointer
	char err[MAXERRMSG];  // buffer for error messages
	int verbose;  // show extra info on query progress
	char *rname;  // name of table/file
	char *qstr;   // query string

	// process command-line args

	if (argc < 3) fatal(USAGE);
	if (strcmp(argv[1], "-v") == 0) {
		verbose = 1;  rname = argv[2];  qstr = argv[3];
	}
	else {
		verbose = 0;  rname = argv[1];  qstr = argv[2];
	}

	if (verbose) { /* keeps compiler quiet */ }

	// initialise relation and scanning structure

	if (!existsRelation(rname)) {
		sprintf(err, "No such relation: %s",rname);
		fatal(err);
	}
	if ((r = openRelation(rname,"r")) == NULL) {
		sprintf(err, "Can't open relation: %s",rname);
		fatal(err);
	}
	if ((q = startQuery(r, qstr)) == NULL) {	
		sprintf(err, "Invalid query: %s",qstr);
		fatal(err);
	}

	// execute the query (find matching tuples)

	char tup[MAXTUPLEN];
	while ((t = getNextTuple(q)) != NULL) {
		tupleString(t,tup);
		printf("%s\n",tup);
	}

	// clean up

	closeQuery(q);
	closeRelation(r);

	return 0;
}

