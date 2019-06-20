typedef struct {
    ushort    nfields;  // # fields
	ushort    dataStart; // 
    FieldDesc fields[]; // field descriptions
//    Record    data;
} Tuple;

Tuple mkTuple(RelnDesc schema, Record record)
{
    int i, pos = 0;
    int size = sizeof(Tuple) +
               nfields*sizeof(FieldDesc);
    Tuple *t = malloc(size+recSize(record));
    t->nfields = schema.nfields;
    for (i=0; i < schema.nfields; i++) {
        int len = record[pos++];
        t->fields[i].offset = pos;
        t->fields[i].length = len;
        // could add checking for over-length fields, etc.
        t->fields[i].type = schema.fields[i].type;
        pos += length;
    }
	t->dataStart = size;
    return t;
}
