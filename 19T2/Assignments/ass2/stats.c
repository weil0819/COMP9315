// stats.c ... a main program that prints info about an MAH relation
// stats.c ... show statistics for a Relation
// part of Multi-attribute linear-hashed files
// Show info and page stats for a Relation
// Usage:  ./stats  RelName

#include "defs.h"
#include "reln.h"

#define USAGE "./stats  RelName"


// Main ... process args, run query

int main(int argc, char **argv)
{
	// process command-line args

	if (argc < 2) fatal(USAGE);
	char *relname = argv[1];

	// open relation and show stats

	if (!existsRelation(relname))
		fatal("No such relation\n");
	Reln r = openRelation(relname,"r");
	if (r == NULL) fatal("No such relation");

	relationStats(r);
	closeRelation(r);

	return 0;
}
