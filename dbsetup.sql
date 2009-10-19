create table rooms( RoomNumber bigint primary key, ShortDesc varchar(82), LongDesc varchar(322), Dir1 bigint, Dir2 bigint, Dir3 bigint, Dir4 bigint, Dir5 bigint, Dir6 bigint, Dir7 bigint, Dir8 bigint, Dir9 bigint, Dir10 bigint);

insert into rooms values( 1, "Room dia Muertos", "The stale air and dank stone make it difficult to breathe. Bones lay piled, who knows how deep they are. Many have died here and more come each day.",0,0,0,0,0,0,0,0,2,0);

create table races( RaceNumber int primary key, RaceName varchar(32), RaceDesc varchar(322));

insert into races values( 1, "Human", "The Human is the most balanced race at this time.");
insert into races values( 1, "Dark-Elf", "The Dark-Elves are a darker relative of the Elf.");
insert into races values( 2, "Dwarf", "The Dwarves are a stout and healthy race, what they lack in height they make up for with sturdiness and brawn.");

create table classes( ClassNumber int primary key, ClassName varchar(32), ClassDesc varchar(322));

insert into classes values( 1, "Fighter", "Physical Combat is thier forte.");
insert into classes values( 2, "Ranger", "Drawn to the woodlands and thier protection, Rangers gain special powers from Nature.");
insert into classes values( 1, "Cleric", "Physical Combat is thier holy calling, and healing thier forte.");
insert into classes values( 1, "Mage", "Frail physically but able to concentrate the mysterious energies into deadly forces.");