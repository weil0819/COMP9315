// insert into a page with fixed-size records
// and distributed free space and slot bitmap

// Assume the following definitions

typedef ... Byte;    // one-byte value
typedef ... Count;   // two-byte value

#define PAGESIZE ...
#define DICTSIZE ...

#define MAXRECS (PAGESIZE-DICTSIZE)/rel.recSize

typedef struct {
	Bit     present[MAXRECS];  // bitmap
	Record  data[1];   // start of tuples
} Page;

// Initialise Page data

void initPage(Relation rel, PageId pid, Page *p)
{
	// p is a pointer to a PAGESIZE buffer
	for (int i = 0; i < MAXRECS; i++)
		p->present[i] = 0;
	put_page(p, rel, pid);
}

// Add record into Page; return record ID

RecordId insert_record(Relation rel, PageId pid, Record rec)
{
	// get Page
	Page *p = get_page(rel, pid);
	// find a free slot
	int slot = -1;
	for (int i = 0; i < MAXRECS; i++)
		if (p->present[i] == 0) {
			slot = i;
			break;
		}
	}
	if (slot < 0) FAIL no room;
	Byte *location = &(p->data) + slot*rel.recSize;
	memcpy(location, r, rel.recSize);
	put_page(p, rel, pid);
	return (pid,slot);
}

// Delete record from Page;

void delete_record(Relation rel, TupleId rid)
{
	(pid,tid) = rid;
	Page *p = get_page(rel, pid);
	p->present[i] = 0;
	put_page(p, rel, pid);
}
