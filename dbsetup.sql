CREATE TABLE rooms( 
       RoomNumber bigint PRIMARY KEY,
       ShortDesc VARCHAR(82) NOT NULL,
       LongDesc VARCHAR(322) NOT NULL,
       north INTEGER NOT NULL,
       northeast INTEGER NOT NULL,
       east INTEGER NOT NULL,
       southeast INTEGER NOT NULL,
       south INTEGER NOT NULL,
       southwest INTEGER NOT NULL,
       west INTEGER NOT NULL,
       northwest INTEGER NOT NULL,
       up INTEGER NOT NULL,
       down INTEGER NOT NULL
);


CREATE TABLE races(
       RaceNumber int PRIMARY KEY,
       RaceName VARCHAR(32) NOT NULL,
       RaceDesc VARCHAR(322) NOT NULL
);



CREATE TABLE classes(
       ClassNumber int PRIMARY KEY,
       ClassName VARCHAR(32) NOT NULL,
       ClassDesc VARCHAR(322) NOT NULL
);

