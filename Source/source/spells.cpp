/***************************************************************
 *           Minions Mud Server is copyright (C)2001           *
 *                    SonzoSoft Software                       *
 *                                                             *
 *         Programmed by Mark Richardson and David Brown       *
 *															   *
 *			 Mark Richardson	-	sinbaud@hotmail.com	       *
 *		       David Brown	    -	dcbrown73@yahoo.com	       *
 ***************************************************************/
#include <string>
#include <map>
#include "sqlite3.h"
#include "ansicolor.h"
#include "spells.h"
#include "player.h"
#include "room.h"
#include "events.h"
#include "scheduler.h"
#include "commands.h"
#include "tcpcomm.h"
#include "combat.h"

using namespace std;

// forward declare
extern MeleeSpellList MeleeSpells;
extern scheduler eventScheduler;
//extern scheduler eventScheduler;
struct Connection;
class Client;
class Room;
class scheduler;
class minionsEvent;


/*==========================================================
############################################################

BaseTimeSpell Functions

############################################################
==========================================================*/
string BaseTimeSpell::GetSpellText(int which)
{
	switch (which)
	{ 
      case INIT_SPELL_TEXT:
		  return SpellInitText;
	  case SPELL_EFFECT_TEXT:
		  return SpellEffectText;
	  case END_EFFECT_TEXT:
		  return SpellEndText;
	  default:
		  ServerLog("Error: BaseTimeSpell::GetSpellText() return default!");
		  return "Default Spell Name";
	}
}

/*==========================================================
############################################################

End of BaseTimeSpell Functions

############################################################
==========================================================*/

bool LoadMeleeSpells( void )
{
	//ClassTable		*CTable = NULL;	
	sqlite3		     	*DatabaseHandle;
	char		    	*SqliteErrorString = 0;
	int			    	SqliteReturnCode, RowCount = 0; //set RowCount to Zero
	sqlite3_stmt		*SqlStatement = 0;
	bool				Finished = false;
	MeleeSpell          *CSpell;

	int                 SpellNumber;
	int					MaxDamage;
	int				    MinDamage;
	int                 ManaCost;
	int                 SpellClass;
	int                 AttackCount;
	string              SpellName;
	string              SpellDesc;
	string              SpellCommand;
	string              SpellString;


	SqliteReturnCode = sqlite3_open( MELEE_SPELLS_DATABASE, &DatabaseHandle);
	
	if( SqliteReturnCode ) //if returns anything but SQLITE_OK some kind of error occurred
	{
		ServerLog( "Can't open database: %s\n", sqlite3_errmsg( DatabaseHandle ) );
		sqlite3_close( DatabaseHandle );
		return false;
	}
	/*  compile the sqlite sql statement  */
	SqliteReturnCode = sqlite3_prepare_v2( DatabaseHandle, "select * from meleespells", -1, &SqlStatement, NULL );

	if( SqliteReturnCode ) //if returns anything other than SQLITE_OK
	{
		ServerLog( "Error in sqlite3_prepare_v2: %s\n", sqlite3_errmsg( DatabaseHandle ) );
		sqlite3_close( DatabaseHandle );
		return false;
	}

	/* we are ready to execute the prepared statement and start getting rows with sqlite3_step() */
	while( !Finished )
	{
		SqliteReturnCode = sqlite3_step( SqlStatement );

		switch( SqliteReturnCode )
		{
		case SQLITE_DONE:		//there are no more rows
			Finished = true;	//this will break the while loop
			break;

		case SQLITE_ROW:		//we have a row of data
			
			/* Grab the result and prepare to create new class object  */
			SpellNumber           = sqlite3_column_int( SqlStatement, 0 );
			SpellClass            = sqlite3_column_int( SqlStatement, 1 );
			AttackCount           = sqlite3_column_int( SqlStatement, 2 );
			SpellCommand          = (char *)sqlite3_column_text( SqlStatement, 3 );
			SpellName             = (char *)sqlite3_column_text( SqlStatement, 4 );
			SpellDesc             = (char *)sqlite3_column_text( SqlStatement, 5 );
			ManaCost              = sqlite3_column_int( SqlStatement, 6 );
			MaxDamage             = sqlite3_column_int( SqlStatement, 7 );
			MinDamage             = sqlite3_column_int( SqlStatement, 8 );
			SpellString           = (char *)sqlite3_column_text( SqlStatement, 9 );

			// Create the new object in the map list, then get a pointer to the new object
			// So we can fill it with the correct spell information
			MeleeSpells.insert( pair<string, MeleeSpell *>(SpellCommand, new MeleeSpell()) );
			CSpell = MeleeSpells.find(SpellCommand)->second;
			// Update the spell object with the spell info
			CSpell->SetSpellNumber( SpellNumber );
			CSpell->SetSpellClass( SpellClass );
			CSpell->SetAttackCount( AttackCount );
			CSpell->SetSpellCommand( SpellCommand );
			CSpell->SetSpellName( SpellName );
			CSpell->SetSpellDescription( SpellDesc );
			CSpell->SetManaCost( ManaCost );
			CSpell->SetMinDamage( MinDamage );
			CSpell->SetMaxDamage( MaxDamage );
			CSpell->SetSpellString( SpellString );

			RowCount++;		//dont forget to increment the row count.
			break;

		default:
			ServerLog( "Serious issue! hit default case in LoadMeleeSpells()" );
			break;
		}	//end of the switch() block of code

	}	//end of the while loop block of code

	sqlite3_finalize( SqlStatement ); //destroy the compiled sqlite statement and free its memory
	sqlite3_close( DatabaseHandle );	//close the database connection.
	ServerLog( "Loaded %i melee spells into memory.", RowCount );
	return true;
}


void CastMeleeSpell( Connection *Player, char *Attacked, MeleeSpell *Spell )
{
	Connection	*Victim = NULL;
	Room		*TempRoom = NULL;

	// Can't be resting if you're even attempting to attack!
	Player->Player.SetRestingStatus(NOT_RESTING);

	if( Attacked == NULL )
	{
		WriteToBuffer( Player, "Usage: %s <target>\n\r", Spell->GetSpellCommand().c_str() );
		return;
	}

	if (Player->Player.GetPlayerWeight() > (Player->Player.GetStrength() *  MAX_STR_MULTIPLIER) )
	{
		WriteToBuffer( Player, "%sYou are to heavy to attack anyone!\n\r%s", ANSI_BR_BLUE, ANSI_WHITE);
		return;
	}

	Victim = SearchForPlayer( Attacked );

	if( !Victim )
	{
		WriteToBuffer( Player, "You don't see %s here.\n\r", Attacked );
		return;
	}
	// If being attacked, you can't rest.
	Victim->Player.SetRestingStatus(NOT_RESTING);
	if( Player == Victim )
	{
		WriteToBuffer( Player, "%sAre you a masochist or what?%s\n\r",
			ANSI_BR_YELLOW, ANSI_WHITE );
		return;
	}

	if( Player->Player.GetRoom() != Victim->Player.GetRoom() )
	{
		WriteToBuffer( Player, "%sYou don't see %s here.%s\n\r",
			ANSI_BR_RED, Attacked, ANSI_WHITE );
		return;
	}
	if (Player->Player.GetAttackEvent())
		StopCombat(Player);

    Player->Victim = Victim;
	Victim->Player.SetRestingStatus(NOT_RESTING);
	// Tell players combat started and create combat event
	DisplayCombatStatus(Player, true);
	minionsEvent *meleeEvent = new meCombatSpell(Player, Victim, Spell);
	Player->Player.SetAttackEvent(meleeEvent);
	eventScheduler.pushCombatStack(meleeEvent);

	return;
}

