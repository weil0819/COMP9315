// dump.c ... a main program that lists all tuples in an MALH relation
// dump.c ... show all tuples in a Relation
// part of Multi-attribute linear-hashed files
// Show tuples, bucket-by-bucket
// Last modified by John Shepherd, July 2019
// Usage:  ./stats  RelName

#include "defs.h"
#include "reln.h"
#include "page.h"

void showAllTuples(Page);

#define USAGE "./dump  RelName"

// Main ... process args, scan data, show tuples

int main(int argc, char **argv)
{
	// process command-line args

	if (argc < 2) fatal(USAGE);
	char *relname = argv[1];

	// open relation and show stats

	if (!existsRelation(relname))
		fatal("No such relation");
	Reln r = openRelation(relname,"r");
	if (r == NULL)
		fatal("Can't open relation");

	for (Offset pid = 0; pid < npages(r); pid++) {
		printf("Bucket[%d]\n",pid);
		// show tuples in data file
		Page pg = getPage(dataFile(r),pid);
		showAllTuples(pg);
		// show tuples in overflow pages
		Page ovpg;  PageID ovp;
		ovp = pageOvflow(pg);
		while (ovp != NO_PAGE) {
			printf("Ovflow->\n");
			ovpg = getPage(ovflowFile(r), ovp);
			showAllTuples(ovpg);
			ovp = pageOvflow(ovpg);
			free(ovpg);
		}
		free(pg);
	}
	closeRelation(r);

	return 0;
}

// scan all tuples in Page

void showAllTuples(Page pg)
{
		Count ntups = pageNTuples(pg);
		char *c = pageData(pg);
		for (int i = 0; i < ntups; i++) {
			printf("%s\n", c);
			c += strlen(c) + 1;
		}
}
