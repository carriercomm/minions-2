/***************************************************************
 *           Minions Mud Server is copyright (C)2001           *
 *                    SonzoSoft Software                       *
 *                                                             *
 *         Programmed by Mark Richardson and David Brown       *
 *															   *
 *			 Mark Richardson	-	sinbaud@hotmail.com	       *
 *		       David Brown	    -	dbrown@centliq.com	       *
 ***************************************************************/
#include <set>

#if !defined RACES_H_INCLUDED
#define RACES_H_INCLUDED

using namespace std;

#define MAX_RACE_NAME                21
#define MAX_CLASS_NAME               21
#define MAX_RACE_DESC			    321
#define MAX_CLASS_DESC  		 	321

const int HP_MULTIPLIER          =    2;
const int MANA_MULTIPLIER        =    2;

#define ROOM_DATABASE			"minions.db"
#define CLASS_DATABASE			"minions.db"
#define RACE_DATABASE			"minions.db"

//Define wearable list type. (vector of integers)
typedef     std::set<int> WearableList;

struct RaceTable
{
	char		RaceName[MAX_RACE_NAME];
	int			RaceNumber;
	char		RaceDesc[MAX_RACE_DESC];
	int         RaceStrength;
	int         RaceAgility;
	int         RaceWisdom;
	int         RaceHealth;
	int         RaceMaxHP;
	int         RaceMaxMana;
	RaceTable  *Next;
};

struct ClassTable
{
	char		   ClassName[MAX_CLASS_NAME];
	int			   ClassNumber;
	char		   ClassDesc[MAX_CLASS_DESC];
	set<int>       WearableTypes;
	set<int>       SpellTypes;
	ClassTable	   *Next;
};


bool LoadRaceTables( void );
bool LoadClassTables( void );
char *LookupRaceStr( int RaceNum );
char *LookupClassStr( int ClassNum );


#endif  //RACES_H_INCLUDED

