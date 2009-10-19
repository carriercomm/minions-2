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

struct RaceTable
{
	char       RaceName[MAX_RACE_NAME];
	int        RaceNumber;
	RaceTable  *Next;
};

struct ClassTable
{
	char       ClassName[MAX_CLASS_NAME];
	int        ClassNumber;
	ClassTable *Next;
};


int LoadRaceTables( void );
int LoadClassTables( void );
char *LookupRaceStr( int RaceNum );
char *LookupClassStr( int ClassNum );

#endif  //RACES_H_INCLUDED

