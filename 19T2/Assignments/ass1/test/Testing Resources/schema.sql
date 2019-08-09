create table Users (
	username    EmailAddr,
	realname    text,
	primary key (username)
);

create table Sessions (
	sessionID   integer,
	username    EmailAddr references Users(username),
	loggedin    timestamp,
	primary key (sessionID)
);


