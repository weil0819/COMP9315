create language plpgsql;

create type TableInfo as ("table" text, ntuples int);

-- show how many tuples are in each table

create or replace function
	pop() returns setof TableInfo
as $$
declare
	r record;
	nr integer;
	res TableInfo;
begin
	for r in select tablename
		 from pg_tables
		 where schemaname = 'public'
		 order by tablename
	loop
		execute 'select count(*) from '
			||quote_ident(r.tablename) into nr;
		res."table" := r.tablename::text;
		res.ntuples := nr::int;
		return next res;
	end loop;
	return;
end;
$$
    language plpgsql;

