create table Users (
	username    text,
	realname    text,
	primary key (username)
);

create table Sessions (
	sessionID   integer,
	username    text references Users(username),
	loggedin    timestamp,
	primary key (sessionID)
);


