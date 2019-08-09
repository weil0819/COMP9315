
-- Some tuples with invalid email addresses
-- Should cause an error message to be generated

\echo
\echo -- Checking invalid emails ... expect to see errors --
\echo

insert into Users values ('x--@abc.com'::EmailAddr, 'Mister X');
insert into Users values ('x.@abc.com'::EmailAddr, 'Mister X');
insert into Users values ('123@abc.com'::EmailAddr,'Mister 123');
insert into Users values ('!$!$#!@abc.com'::EmailAddr,'Mister naughty');
insert into Users values ('jas@cse'::EmailAddr,'Mister JAS');
insert into Users values ('jas@cse.'::EmailAddr,'Mister Trailing Dot');
insert into Users values ('jas@cse@unsw'::EmailAddr,'Mister Two Ats');

-- should return true

\echo
\echo -- Checking relational operators ... tests below should all return True --
\echo

select 'jas@abc.com'::EmailAddr = 'JAS@aBc.CoM'::EmailAddr;
select 'jas@abc.com'::EmailAddr <= 'jas@abc.com'::EmailAddr;
select 'xx1@abc.com'::EmailAddr < 'xx2@abc.com'::EmailAddr;
select 'xx2@abc.com'::EmailAddr > 'xx1@abc.com'::EmailAddr;
select 'xx2@abc.com'::EmailAddr < 'xx1@abd.com'::EmailAddr;
select 'xx2@abc.com'::EmailAddr <> 'xx1@abd.com'::EmailAddr;

-- should return false

\echo
\echo -- Checking relational operators ... tests below should all return False --
\echo

select 'jas@abc.com'::EmailAddr <> 'JAS@aBc.CoM'::EmailAddr;
select 'jas@abc.co'::EmailAddr = 'jas@abc.com'::EmailAddr;
select 'jas@abc.com'::EmailAddr > 'jas@abc.com'::EmailAddr;
select 'xx1@abc.com'::EmailAddr > 'xx2@abc.com'::EmailAddr;
select 'xx2@abc.com'::EmailAddr > 'xx1@abd.com'::EmailAddr;

\echo
\echo -- Running some queries --
\echo
\timing on

\echo
\echo -- Count users/sessions ...
\echo -- There should be 68988 Users and 111002 Sessions
\echo
select count(*) from Users;
\echo
select count(*) from Sessions;

\echo
\echo -- Lookup on Users using EmailAddr
\echo
select * from Users
where username='chang.lee@saintmarys.edu'::EmailAddr;

\echo
\echo -- Lookup on Sessions using EmailAddr
\echo
select * from Sessions
where  username='t.kawamura@a1b2c3.enit.fr'::EmailAddr;

\echo
\echo -- Check ordering based on domain, then user
\echo
select * from Sessions order by username limit 20;

\echo
\echo -- Check grouping based on email
\echo
select username, count(*)
from   Sessions group by username having count(*) > 3
limit 20;

\echo
\echo -- Join on EmailAddres
\echo
select u.realname, s.loggedin
from   Users u join Sessions s on (u.username=s.username)
where  s.username = 'd.fleck@a1b2c3.enit.fr';

\timing off

-- Check if it can generate and use indexes

\echo
\echo -- Check to see whether hash index works for Sessions.username --
\echo

create index hash_file on Sessions using hash (username);

explain (analyze, format yaml)
select * from Sessions
where username = 'p.toft@inbox.vsh.cz'::EmailAddr;

drop index hash_file;
