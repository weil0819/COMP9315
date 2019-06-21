-- "pg_class" holds core information about tables
-- "pg_attribute" contains information about attributes
-- "pg_namespace" contains information about schemata

create or replace function schema() returns setof text
as $$
declare
        rec record;
        rel text := '';
        att text := '';
        out text := '';
		len integer := 0;
begin
	for rec in
		select t.relname, a.attname     -- Name of the table, The column name
		from   pg_class t, pg_attribute a, pg_namespace n
		where  t.relkind='r'			-- r=ordinary table
			and t.relnamespace = n.oid  -- The OID of the namespace that contains this relation, pg_namespace.oid
			and n.nspname = 'public'    -- Name of the namespace
			and a.attrelid = t.oid      -- The table this column belongs to, pg_class.oid
			and a.attnum > 0            -- The number of the column
		order by t.relname, a.attnum         
	loop
		if (rec.relname <> rel) then    -- If table name is different from the former one
			if (rel <> '') then         -- If relation name is non-null
				out := rel || '(' || att || ')';
				return next out;
			end if;
			rel := rec.relname;         -- relation name = table name 
			att := '';
			len := 0;
		end if;
		if (att <> '') then             -- If attribute-list is non-null
			att := att || ', ';         -- Append attribute to attribute-list
			len := len + 2;             -- string length + 2
		end if;
		if (len + length(rec.attname) > 70) then -- newline
			att := att || E'\n        ';
			len := 0;
		end if;
		att := att || rec.attname;
		len := len + length(rec.attname);
	end loop;
	-- deal with last table
	if (rel <> '') then
		out := rel || '(' || att || ')';
		return next out;
	end if;
end;
$$ language plpgsql;
