\echo
\echo -- Check to see whether very long addresses were inserted
\echo

select * from Users where realname = 'Mister X1';
select * from Users where realname = 'Mister X2';

\echo
\echo -- This tuple has a too-long email address
\echo

select * from Users where realname = 'Mister X3';
