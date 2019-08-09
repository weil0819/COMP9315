// reln.h, reln.c ... an ADT for relations
// reln.c ... functions on Relations
// part of Multi-attribute Linear-hashed Files
// Last modified by John Shepherd, July 2019

#include "defs.h"
#include "reln.h"
#include "page.h"
#include "tuple.h"
#include "chvec.h"
#include "bits.h"
#include "hash.h"

#define HEADERSIZE (3*sizeof(Count)+sizeof(Offset))

struct RelnRep {
	Count  nattrs; // number of attributes
	Count  depth;  // depth of main data file
	Offset sp;     // split pointer
    Count  npages; // number of main data pages
    Count  ntups;  // total number of tuples
	ChVec  cv;     // choice vector
	char   mode;   // open for read/write
	FILE  *info;   // handle on info file
	FILE  *data;   // handle on data file
	FILE  *ovflow; // handle on ovflow file
};

// create a new relation (three files)

Status newRelation(char *name, Count nattrs, Count npages, Count d, char *cv)
{
    char fname[MAXFILENAME];
	Reln r = malloc(sizeof(struct RelnRep));
	r->nattrs = nattrs; r->depth = d; r->sp = 0;
	r->npages = npages; r->ntups = 0; r->mode = 'w';
	assert(r != NULL);
	if (parseChVec(r, cv, r->cv) != OK) return ~OK;
	sprintf(fname,"%s.info",name);
	r->info = fopen(fname,"w");
	assert(r->info != NULL);
	sprintf(fname,"%s.data",name);
	r->data = fopen(fname,"w");
	assert(r->data != NULL);
	sprintf(fname,"%s.ovflow",name);
	r->ovflow = fopen(fname,"w");
	assert(r->ovflow != NULL);
	int i;
	for (i = 0; i < npages; i++) addPage(r->data);
	closeRelation(r);
	return 0;
}

// check whether a relation already exists

Bool existsRelation(char *name)
{
	char fname[MAXFILENAME];
	sprintf(fname,"%s.info",name);
	FILE *f = fopen(fname,"r");
	if (f == NULL)
		return FALSE;
	else {
		fclose(f);
		return TRUE;
	}
}

// set up a relation descriptor from relation name
// open files, reads information from rel.info

Reln openRelation(char *name, char *mode)
{
	Reln r;
	r = malloc(sizeof(struct RelnRep));
	assert(r != NULL);
	char fname[MAXFILENAME];
	sprintf(fname,"%s.info",name);
	r->info = fopen(fname,mode);
	assert(r->info != NULL);
	sprintf(fname,"%s.data",name);
	r->data = fopen(fname,mode);
	assert(r->data != NULL);
	sprintf(fname,"%s.ovflow",name);
	r->ovflow = fopen(fname,mode);
	assert(r->ovflow != NULL);
	// Naughty: assumes Count and Offset are the same size
	int n = fread(r, sizeof(Count), 5, r->info);
	assert(n == 5);
	n = fread(r->cv, sizeof(ChVecItem), MAXCHVEC, r->info);
	assert(n == MAXCHVEC);
	r->mode = (mode[0] == 'w' || mode[1] =='+') ? 'w' : 'r';
	return r;
}

// release files and descriptor for an open relation
// copy latest information to .info file

void closeRelation(Reln r)
{
	// make sure updated global data is put in info
	// Naughty: assumes Count and Offset are the same size
	if (r->mode == 'w') {
		fseek(r->info, 0, SEEK_SET);
		// write out core relation info (#attr,#pages,d,sp)
		int n = fwrite(r, sizeof(Count), 5, r->info);
		assert(n == 5);
		// write out choice vector
		n = fwrite(r->cv, sizeof(ChVecItem), MAXCHVEC, r->info);
		assert(n == MAXCHVEC);
	}
	fclose(r->info);
	fclose(r->data);
	fclose(r->ovflow);
	free(r);
}

// insert a new tuple into a relation
// returns index of bucket where inserted
// - index always refers to a primary data page
// - the actual insertion page may be either a data page or an overflow page
// returns NO_PAGE if insert fails completely
// TODO: include splitting and file expansion

PageID addToRelation(Reln r, Tuple t)
{
	// Step-1: Acquire composite hash.
	Bits h, p;
	// char buf[MAXBITS+1];
	h = tupleHash(r,t);			// get composite hash as page address

	// Step-2: Compute page address.
	if (r->depth == 0)
		p = 1;
	else {
		p = getLower(h, r->depth);	// extract the lowest depth bits
		if (p < r->sp) p = getLower(h, r->depth+1);	// if page address is left of sp, extract d+1 bits
	}
	// bitsString(h,buf); printf("hash = %s\n",buf);
	// bitsString(p,buf); printf("page = %s\n",buf);

	// Step-3: Get Page object from file according to pageID.
	Page pg = getPage(r->data,p);

	// Step-4: Can add new tuple into above Page.
	if (addToPage(pg,t) == OK) {
		putPage(r->data,p,pg);	// write a Page to a file since current page has new tuple
		r->ntups++;				// #tuples + 1
		return p;				// p is pageID or page address
	}

	// Step-5: Can not add new tuple into above Page.
	// Step-5-1: Current Page has no overflow page.
	if (pageOvflow(pg) == NO_PAGE) {	// primary data page full
		// Step-5-1-1: Add first overflow page in chain in overflow file.
		PageID newp = addPage(r->ovflow);

		// Step-5-1-2: Set overflow page address(ID) to current page.
		pageSetOvflow(pg,newp);			// pg is current Page

		// Step-5-1-3: Write a Page to a file due to current Page has a new overflow chain.
		putPage(r->data,p,pg);			// r->data is data file

		// Step-5-1-4: Get Page object from overflow file according to pageID.
		Page newpg = getPage(r->ovflow,newp);	// newp is Page ID

		// Step-5-1-6: Try to insert new tuple into new created overflow Page.
		// can't add to a new page; we have a problem
		if (addToPage(newpg,t) != OK) return NO_PAGE;

		// Step-5-1-7: Write a Page to a file due to current Page has a new tuple.
		putPage(r->ovflow,newp,newpg);
		r->ntups++;
		return p;
	}
	// Step-5-2: Current Page has overflow page.
	else {
		// Step-5-2-1: scan overflow chain until we find space
		// worst case: add new ovflow page at end of chain 
		Page ovpg, prevpg = NULL;				// prevg is previous Page object for new added overflow
		PageID ovp, prevp = NO_PAGE;
		ovp = pageOvflow(pg);					// pg is current Page object
		while (ovp != NO_PAGE) {				// ovp is overflow PageID of current Page
			ovpg = getPage(r->ovflow, ovp);		// get overflow Page object
			// Case-I: new tuple can not be added into current oveflow Page.
			if (addToPage(ovpg,t) != OK) {		// add new tuple into overflow Page
				prevp = ovp; prevpg = ovpg;		// move to next level overflow Page
				ovp = pageOvflow(ovpg);			// get next level overflow PageID of overflow Page
			}
			// Case-II: new tuple can be added into current overflow Page.
			else {
				if (prevpg != NULL) free(prevpg);	// release last level Page object
				putPage(r->ovflow,ovp,ovpg);		// write update overflow Page back to overflow file
				r->ntups++;
				return p;
			}
		}

		// Step-5-2-2: all overflow pages are full; add another to chain
		// at this point, there *must* be a prevpg
		assert(prevpg != NULL);		
		PageID newp = addPage(r->ovflow);			// make new overflow page		
		Page newpg = getPage(r->ovflow,newp);		// insert tuple into new page
        if (addToPage(newpg,t) != OK) return NO_PAGE;
        putPage(r->ovflow,newp,newpg);				// update new overflow page	
		pageSetOvflow(prevpg,newp);					// link to existing overflow chain
		putPage(r->ovflow,prevp,prevpg);			// update previous overflow page
        r->ntups++;
		return p;
	}

	// Step-6: Decide splitting or not.
	int c = (int)1024/(10*r->nattrs);	// calculate capacity value--c

	// Step-6-1: File expands after every c insertions.
	if(r->ntups % c == 0) {
		// Step-6-2: Calculate newp = sp + 2^d & oldp = sp, they are both PageID.
		Bits newp, oldp;
		oldp = r->sp;
		newp = r->sp + (1 << r->depth);
		addPage(r->data);					// add a new data page in data file

		// Step-6-3: Iterate each tuple in Page[oldp].
		Page oldpg = getPage(r->data,oldp);	// get Page[oldp] from data file
		Count ntups = pageNTuples(oldpg);	// how many tuples in this Page
		char *startData;
		startData = pageData(oldpg);		// tuples string, comma-separate
		int i;
		for(i = 0; i < ntups; i++) {
			Tuple tup = startData;				// get tuple
			Bits ha = tupleHash(r,tup);			// get tuple hash	
			Bits pp = getLower(ha, r->depth+1);	// get new PageID
			if(pp == newp) {					// add tuple tup to bucket[newp]
				// Case-I: can insert into Page[newp]				
				Page newpg = getPage(r->data,newp);	// fetch a Page object from data file 
				if (addToPage(newpg,tup) == OK) {
					putPage(r->data,newp,newpg);	// write a Page to a file since current page has new tuple
				}

				// Case-II: can not insert into Page[newp], and no overflow page on it
				if (pageOvflow(newpg) == NO_PAGE) { 
					PageID newovp = addPage(r->ovflow);			// add a new overflow page in overflow file 
					pageSetOvflow(newpg,newovp);				// pg is current Page
					putPage(r->data,newp,newpg);				// r->data is data file
					Page newovpg = getPage(r->ovflow,newovp);	// create a Page object for overflow page
					if (addToPage(newovpg,tup) != OK) return NO_PAGE;
					putPage(r->ovflow,newovp,newovpg);
				} 
				// Case-III: can not insert into Page[newp], and has overflow page on it
				else {
					// scan overflow chain until we find space
					// worst case: add new ovflow page at end of chain 
					Page ovpg, prevpg = NULL;				// prevg is previous Page object for new added overflow
					PageID ovp, prevp = NO_PAGE;
					ovp = pageOvflow(newpg);				// pg is current Page object
					while (ovp != NO_PAGE) {				// ovp is overflow PageID of current Page
						ovpg = getPage(r->ovflow, ovp);		// get overflow Page object
						// Case-I: new tuple can not be added into current oveflow Page.
						if (addToPage(ovpg,tup) != OK) {	// add new tuple into overflow Page
							prevp = ovp; prevpg = ovpg;		// move to next level overflow Page
							ovp = pageOvflow(ovpg);			// get next level overflow PageID of overflow Page
						}
						// Case-II: new tuple can be added into current overflow Page.
						else {
							if (prevpg != NULL) free(prevpg);	// release last level Page object
							putPage(r->ovflow,ovp,ovpg);		// write update overflow Page back to overflow file
						}
					}

					// all overflow pages are full; add another to chain
					// at this point, there *must* be a prevpg
					assert(prevpg != NULL);		
					PageID newp = addPage(r->ovflow);			// make new overflow page		
					Page newpg = getPage(r->ovflow,newp);		// insert tuple into new page
        			if (addToPage(newpg,tup) != OK) return NO_PAGE;
        			putPage(r->ovflow,newp,newpg);				// update new overflow page	
					pageSetOvflow(prevpg,newp);					// link to existing overflow chain
					putPage(r->ovflow,prevp,prevpg);			// update previous overflow page
				}			
			}
			startData = startData + tupLength(startData) + 1;	// move pointer
		}

		// Step-6-4: Iterate each tuple in Page[oldp]'s multi-level overflow pages.
		PageID oldovp = pageOvflow(oldpg);
		while (oldovp != NO_PAGE) {				// ovp is overflow PageID of current Page
			oldovpg = getPage(r->ovflow, oldovp);		// get overflow Page object
			Count ntups = pageNTuples(oldovpg);	// how many tuples in this Page
			char *startData;
			startData = pageData(oldovpg);		// tuples string, comma-separate
			int i;
			for(i = 0; i < ntups; i++) {
				Tuple tup = startData;				// get tuple
				Bits ha = tupleHash(r,tup);			// get tuple hash	
				Bits pp = getLower(ha, r->depth+1);	// get new PageID 
				if(pp == newp) {					// add tuple tup to bucket[newp]
					// Case-I: can insert into Page[newp]				
					Page newpg = getPage(r->data,newp);	// fetch a Page object from data file 
					if (addToPage(newpg,tup) == OK) {
						putPage(r->data,newp,newpg);	// write a Page to a file since current page has new tuple
					}

					// Case-II: can not insert into Page[newp], and no overflow page on it
					if (pageOvflow(newpg) == NO_PAGE) { 
						PageID newovp = addPage(r->ovflow);			// add a new overflow page in overflow file 
						pageSetOvflow(newpg,newovp);				// pg is current Page
						putPage(r->data,newp,newpg);				// r->data is data file
						Page newovpg = getPage(r->ovflow,newovp);	// create a Page object for overflow page
						if (addToPage(newovpg,tup) != OK) return NO_PAGE;
						putPage(r->ovflow,newovp,newovpg);
					} 
					// Case-III: can not insert into Page[newp], and has overflow page on it
					else {
						// scan overflow chain until we find space
						// worst case: add new ovflow page at end of chain 
						Page ovpg, prevpg = NULL;				// prevg is previous Page object for new added overflow
						PageID ovp, prevp = NO_PAGE;
						ovp = pageOvflow(newpg);				// pg is current Page object
						while (ovp != NO_PAGE) {				// ovp is overflow PageID of current Page
							ovpg = getPage(r->ovflow, ovp);		// get overflow Page object
							// Case-I: new tuple can not be added into current oveflow Page.
							if (addToPage(ovpg,tup) != OK) {	// add new tuple into overflow Page
								prevp = ovp; prevpg = ovpg;		// move to next level overflow Page
								ovp = pageOvflow(ovpg);			// get next level overflow PageID of overflow Page
							}
							// Case-II: new tuple can be added into current overflow Page.
							else {
								if (prevpg != NULL) free(prevpg);	// release last level Page object
								putPage(r->ovflow,ovp,ovpg);		// write update overflow Page back to overflow file
							}
						}

						// all overflow pages are full; add another to chain
						// at this point, there *must* be a prevpg
						assert(prevpg != NULL);		
						PageID newp = addPage(r->ovflow);			// make new overflow page		
						Page newpg = getPage(r->ovflow,newp);		// insert tuple into new page
        				if (addToPage(newpg,tup) != OK) return NO_PAGE;
        				putPage(r->ovflow,newp,newpg);				// update new overflow page	
						pageSetOvflow(prevpg,newp);					// link to existing overflow chain
						putPage(r->ovflow,prevp,prevpg);			// update previous overflow page
					}			
				}
				startData = startData + tupLength(startData) + 1;	// move pointer
			}
		}

		r->sp = r->sp + 1;
		if(r->sp == (1 << r->depth)) {
			r->depth = r->depth + 1;
			r->sp = 0;
		}
	}

	return NO_PAGE;
}

// external interfaces for Reln data

FILE *dataFile(Reln r) { return r->data; }
FILE *ovflowFile(Reln r) { return r->ovflow; }
Count nattrs(Reln r) { return r->nattrs; }
Count npages(Reln r) { return r->npages; }
Count ntuples(Reln r) { return r->ntups; }
Count depth(Reln r)  { return r->depth; }
Count splitp(Reln r) { return r->sp; }
ChVecItem *chvec(Reln r)  { return r->cv; }


// displays info about open Reln

void relationStats(Reln r)
{
	printf("Global Info:\n");
	printf("#attrs:%d  #pages:%d  #tuples:%d  d:%d  sp:%d\n",
	       r->nattrs, r->npages, r->ntups, r->depth, r->sp);
	printf("Choice vector\n");
	printChVec(r->cv);
	printf("Bucket Info:\n");
	printf("%-4s %s\n","#","Info on pages in bucket");
	printf("%-4s %s\n","","(pageID,#tuples,freebytes,ovflow)");
	for (Offset pid = 0; pid < r->npages; pid++) {
		printf("[%2d]  ",pid);
		Page p = getPage(r->data, pid);
		Count ntups = pageNTuples(p);
		Count space = pageFreeSpace(p);
		Offset ovid = pageOvflow(p);
		printf("(d%d,%d,%d,%d)",pid,ntups,space,ovid);
		free(p);
		while (ovid != NO_PAGE) {
			Offset curid = ovid;
			p = getPage(r->ovflow, ovid);
			ntups = pageNTuples(p);
			space = pageFreeSpace(p);
			ovid = pageOvflow(p);
			printf(" -> (ov%d,%d,%d,%d)",curid,ntups,space,ovid);
			free(p);
		}
		putchar('\n');
	}
}
