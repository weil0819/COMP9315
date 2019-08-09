-- How many users/sessions are there?

select count(*) from Users;


-- How many sessions has each user had?

select username, count(*)
from   Sessions group by username having count(*) > 1;


-- Produce a list of users' real names and session time

select u.realname, s.loggedin
from   Users u join Sessions s using (username)
order  by s.loggedin;


-- Some tuples with invalid email addresses
-- Should cause an error message to be generated
-- You should think of other cases to test

insert into Users values
('x--@abc.com'::EmailAddr, 'Mister X');
insert into Users values
('123@abc.com'::EmailAddr,'Mister 123');
insert into Users values
('!$!$#!@abc.com'::EmailAddr,'Mister naughty');
insert into Users values
('jas@cse'::EmailAddr,'Mister JAS');


-- Test some of the comparison operators
-- You should think of other cases to test

-- should return true

select 'jas@abc.com'::EmailAddr = 'JAS@aBc.CoM'::EmailAddr;
select 'xx1@abc.com'::EmailAddr < 'xx2@abc.com'::EmailAddr;
select 'xx2@abc.com'::EmailAddr < 'xx1@abd.com'::EmailAddr;

-- should return false

select 'jas@abc.com'::EmailAddr <> 'JAS@aBc.CoM'::EmailAddr;
