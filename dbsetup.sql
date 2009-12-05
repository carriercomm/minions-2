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
       RaceDesc VARCHAR(322) NOT NULL,
       Strength int NOT NULL,
       Agility int NOT NULL,
       Wisdom int NOT NULL,
       Health int NOT NULL
);

CREATE TABLE classes(
       ClassNumber int PRIMARY KEY,
       ClassName VARCHAR(32) NOT NULL,
       ClassDesc VARCHAR(322) NOT NULL,
       WearableTypes  VARCHAR(100) NOT NULL,
       SpellTypes VARCHAR(100) NOT NULL,
       ClassGesture VARCHAR(75) NOT NULL,
       ClassMyGesture VARCHAR(75) NOT NULL
);

CREATE TABLE items(
	ItemNumber int PRIMARY KEY,
	Value int NOT NULL,
	ItemType int NOT NULL,
	Weight int NOT NULL,
	ItemName VARCHAR(35) NOT NULL,
	ItemDesc VARCHAR(201) NOT NULL,
	MaxDamage int NOT NULL,
	ToHitBonus int NOT NULL,
	Speed int NOT NULL,
	ArmorValue int NOT NULL,
	AttackType VARCHAR(21) NOT NULL,
	MinDamage int NOT NULL,
	StrBonus int NOT NULL,
	AgilBonus int NOT NULL,
        HealthBonus int NOT NULL,
        LuckBonus int NOT NULL,
        WisdomBonus int NOT NULL,
        HitPointsBonus int NOT NULL,
        ManaBonus int NOT NULL,
	WearLocation int NOT NULL,
	WearableType int NOT NULL
);

CREATE TABLE meleespells(
	SpellNumber int PRIMARY KEY,
	SpellClass int NOT NULL,
	AttackCount int NOT NULL,
	SpellCommand VARCHAR(4) NOT NULL,
	SpellName VARCHAR(35) NOT NULL,
	SpellDesc VARCHAR(201) NOT NULL,
        ManaCost int NOT NULL,
	MaxDamage int NOT NULL,
	MinDamage int NOT NULL,
	SpellString VARCHAR(21) NOT NULL
);