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
		select relname,attname
		from   pg_class t, pg_attribute a, pg_namespace n
		where  t.relkind='r'
			and t.relnamespace = n.oid
			and n.nspname = 'public'
			and attrelid = t.oid
			and attnum > 0
		order by relname,attnum
	loop
		if (rec.relname <> rel) then
			if (rel <> '') then
				out := rel || '(' || att || ')';
				return next out;
			end if;
			rel := rec.relname;
			att := '';
			len := 0;
		end if;
		if (att <> '') then
			att := att || ', ';
			len := len + 2;
		end if;
		if (len + length(rec.attname) > 70) then
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
