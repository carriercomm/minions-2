/***************************************************************
 *           Minions Mud Server is copyright (C)2001           *
 *                    SonzoSoft Software                       *
 *                                                             *
 *         Programmed by Mark Richardson and David Brown       *
 *															   *
 *			 Mark Richardson	-	sinbaud@hotmail.com	       *
 *		       David Brown	    -	dbrown@centliq.com	       *
 ***************************************************************/
#if !defined RACES_H_INCLUDED
#define RACES_H_INCLUDED

#define MAX_RACE_NAME          21
#define MAX_CLASS_NAME         21
#define MAX_RACE_DESC			80
#define MAX_CLASS_DESC			80
#define ROOM_DATABASE			"c:\\builds\\minions\\minions.db"
#define CLASS_DATABASE			"c:\\builds\\minions\\minions.db"
#define RACE_DATABASE			"c:\\builds\\minions\\minions.db"

struct RaceTable
{
	char		RaceName[MAX_RACE_NAME];
	int			RaceNumber;
	char		RaceDesc[MAX_RACE_DESC];
	RaceTable  *Next;
};

struct ClassTable
{
	char		ClassName[MAX_CLASS_NAME];
	int			ClassNumber;
	char		ClassDesc[MAX_CLASS_DESC];
	ClassTable	*Next;
};


bool LoadRaceTables( void );
bool LoadClassTables( void );
char *LookupRaceStr( int RaceNum );
char *LookupClassStr( int ClassNum );

#endif  //RACES_H_INCLUDED

