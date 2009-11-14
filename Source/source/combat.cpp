/***************************************************************
 *           Minions Mud Server is copyright (C)2001           *
 *                    SonzoSoft Software                       *
 *                                                             *
 *         Programmed by Mark Richardson and David Brown       *
 *															   *
 *			 Mark Richardson	-	sinbaud@hotmail.com	       *
 *		       David Brown	    -	dcbrown73@yahoo.com	       *
 ***************************************************************/
#include <stdlib.h>
#include <vector>
#include <map>
#include "combat.h"
#include "events.h"
#include "scheduler.h"
#include "room.h"
#include "ansicolor.h"
#include "tcpcomm.h"
#include "item.h"



using namespace std;
/**********************************************************
 *   Global Variables                                     *
 **********************************************************/
extern Connection	*PlayerList;
CombatList			*FirstCombantant = '\0';


/**********************************************************
 *   Functions                                            *
 **********************************************************/
//void baseCombat( void )
//{


void DoCombatRound( void )
{
	Connection  *TempConn = '\0';
	Room		*TempRoom = '\0';
	Room		*VictimRoom = '\0';
	Item		*TempItem = '\0';
	int			Damage = 0;
	int			ToHitRoll = 0;
	int			ToHitValue = 0;
	int			AC = 0;
	int			Thac0 = 0;

	for( TempConn = PlayerList; TempConn; TempConn = TempConn->Next )
	{
		if( TempConn->Victim )
		{
			TempRoom = TempConn->Player.GetRoom();
			VictimRoom = TempConn->Victim->Player.GetRoom();
			TempItem = TempConn->Player.GetWieldedItem();
			AC = TempConn->Player.GetArmorClass();
			Thac0 = TempConn->Player.GetTHAC0();
			ToHitValue = Thac0 - AC;
			ToHitRoll = rand() % 20 + 1;
			
			if( TempRoom != VictimRoom )
			{
				
				WriteToBuffer( TempConn, "%s*** Combat Off ***%s\n\r",
					ANSI_YELLOW, ANSI_WHITE );
				WriteToBuffer( TempConn->Victim, "%s*** Combat Off ***%s\n\r",
					ANSI_YELLOW, ANSI_WHITE );
				TempConn->Victim = '\0';
				
				continue;
			}

			/*  if its a crit */
			if( ToHitRoll > 16 )
			{
				if( TempItem )
				{
					Damage = ( rand() % TempItem->GetMaxDamage() ) + TempItem->GetMaxDamage();

					/* one for the hitter */
					WriteToBuffer( TempConn, "%sYou critically %s %s with your %s for %d damage!%s\n\r",
						ANSI_BR_RED, TempItem->GetAttackType(), TempConn->Victim->Player.GetFirstName(),
						TempItem->GetItemName(), Damage, ANSI_WHITE );

					/* one for the Hit-ee  */
					WriteToBuffer( TempConn->Victim, "%s%s critically %ss you with his %s for %d damage!%s\n\r",
						ANSI_BR_RED, TempConn->Player.GetFirstName(), TempItem->GetAttackType(),
						TempItem->GetItemName(), Damage, ANSI_WHITE );

					/*  and one for everyone else in the room */
					TempRoom->SelectiveBroadcast( TempConn, TempConn->Victim, "%s%s critically %ss %s with his %s for %d damage!%s\n\r",
						ANSI_BR_RED, TempConn->Player.GetFirstName(), TempItem->GetAttackType(),
						TempConn->Victim->Player.GetFirstName(), TempItem->GetItemName(),
						Damage, ANSI_WHITE );
				}

				else
				{
					Damage = ( rand() % 3 ) * 3;

					/* one for the hitter */
					WriteToBuffer( TempConn, "%sYou critically punch %s for %d damage!%s\n\r",
						ANSI_BR_RED, TempConn->Victim->Player.GetFirstName(),
						Damage, ANSI_WHITE );

					/* one for the Hit-ee  */
					WriteToBuffer( TempConn->Victim, "%s%s critically punches you for %d damage!%s\n\r",
						ANSI_BR_RED, TempConn->Player.GetFirstName(), Damage, ANSI_WHITE );

					/*  and one for everyone else in the room */
					TempRoom->SelectiveBroadcast( TempConn, TempConn->Victim, "%s%s critically punches %s for %d damage!%s\n\r",
						ANSI_BR_RED, TempConn->Player.GetFirstName(), TempConn->Victim->Player.GetFirstName(), Damage,
						ANSI_WHITE );
				}

				if(!TempConn->Victim->Player.UpdateHitPoints( Damage, false ))
				{ 
					
					Die( TempConn->Victim, TempRoom );
					TempConn->Player.AddKill();
                    TempConn->Victim = '\0';
					continue;
				}

				continue;
			}

			/*  else do normal damage  */
			if( ToHitRoll >= ToHitValue )
			{
				if( TempItem )
				{
					Damage = ( rand() % TempItem->GetMaxDamage() ) + TempItem->GetMinDamage();

					/* one for the hitter */
					WriteToBuffer( TempConn, "%sYou %s %s with your %s for %d damage!%s\n\r",
						ANSI_BR_RED, TempItem->GetAttackType(), TempConn->Victim->Player.GetFirstName(),
						TempItem->GetItemName(), Damage, ANSI_WHITE );

					/* one for the Hit-ee  */
					WriteToBuffer( TempConn->Victim, "%s%s %ss you with his %s for %d damage!%s\n\r",
						ANSI_BR_RED, TempConn->Player.GetFirstName(), TempItem->GetAttackType(),
						TempItem->GetItemName(), Damage, ANSI_WHITE );

					/*  and one for everyone else in the room */
					TempRoom->SelectiveBroadcast( TempConn, TempConn->Victim, "%s%s %ss %s with his %s for %d damage!%s\n\r",
						ANSI_BR_RED, TempConn->Player.GetFirstName(), TempItem->GetAttackType(),
						TempConn->Victim->Player.GetFirstName(), TempItem->GetItemName(),
						Damage, ANSI_WHITE );
				}

				else
				{
					Damage = ( rand() % 3 ) + 1;

					/* one for the hitter */
					WriteToBuffer( TempConn, "%sYou punch %s for %d damage!%s\n\r",
						ANSI_BR_RED, TempConn->Victim->Player.GetFirstName(),
						Damage, ANSI_WHITE );

					/* one for the Hit-ee  */
					WriteToBuffer( TempConn->Victim, "%s%s punches you for %d damage!%s\n\r",
						ANSI_BR_RED, TempConn->Player.GetFirstName(), Damage, ANSI_WHITE );

					/*  and one for everyone else in the room */
					TempRoom->SelectiveBroadcast( TempConn, TempConn->Victim, "%s%s punches %s for %d damage!%s\n\r",
						ANSI_BR_RED, TempConn->Player.GetFirstName(), TempConn->Victim->Player.GetFirstName(), Damage,
						ANSI_WHITE );
				}

				if(!TempConn->Victim->Player.UpdateHitPoints( Damage, false ))
				{
					Die( TempConn->Victim, TempRoom );
					TempConn->Player.AddKill();
					TempConn->Victim->Player.AddBeenKilled();
					TempConn->Victim = '\0';
					continue;
				}

				continue;
			
			}

			/*  If the Hit Roll missed  */
			else if( TempItem )
			{
				/* one for the hitter */
				WriteToBuffer( TempConn, "%sYou swing at %s with your %s but miss!\n\r",
					ANSI_WHITE, TempConn->Victim->Player.GetFirstName(),
					TempItem->GetItemName() );

				/* one for the Hit-ee  */
				WriteToBuffer( TempConn->Victim, "%s%s swings at you with an %s but misses!\n\r",
					ANSI_WHITE, TempConn->Player.GetFirstName(), TempItem->GetItemName() );

				/*  and one for everyone else in the room */
				TempRoom->SelectiveBroadcast( TempConn, TempConn->Victim, "%s%s swings at %s with an %s but misses!\n\r",
					ANSI_WHITE, TempConn->Player.GetFirstName(), TempConn->Victim->Player.GetFirstName(),
					TempItem->GetItemName() );
				
			}

			else
			{
				/* one for the hitter */
				WriteToBuffer( TempConn, "%sYou swing at %s but miss!\n\r",
					ANSI_WHITE, TempConn->Victim->Player.GetFirstName() );

				/* one for the Hit-ee  */
				WriteToBuffer( TempConn->Victim, "%s%s swings at you but misses!\n\r",
					ANSI_WHITE, TempConn->Player.GetFirstName() );

				/*  and one for everyone else in the room */
				TempRoom->SelectiveBroadcast( TempConn, TempConn->Victim, "%s%s swings at %s but misses!\n\r",
					ANSI_WHITE, TempConn->Player.GetFirstName(), TempConn->Victim->Player.GetFirstName() );
			}
		}
	}

	return;
}

/**********************************************************
 *  Called when some poor sap bites the big one           *
 **********************************************************/
void Die( Connection *Player, Room *CurRom )
{	
	Room			*Rom = '\0';
	ItemsOnPlayer	*ListPtr = '\0';
	ItemsOnPlayer	*ListPtr_Next = '\0';
		
	if (!CurRom || !Player )
	{
		WriteToBuffer( Player, "Fuck up in Die(), Null room pointer CurRom.\n\r");
		return;
	}

	/* when ya die yer no longer engaged on anyone */
	StopCombat(Player);
	/*Player->Victim = '\0';

	// Kill any attack event in the queue!
	if (Player->Player.GetAttackEvent())
	{	
		Player->Player.GetAttackEvent()->killEvent();
		Player->Player.SetAttackEvent('\0');

	}
	*/
	/*  get the respawn room */
	Rom = SearchForRoom( 1 );
	
	if( !Rom )
	{
		WriteToBuffer( Player, "Fuck up in Die(), Null room pointer Rom.\n\r");
		return;
	}

	Player->Player.DropAllItems();
	
	CurRom->RemovePlayerFromRoom( Player );

	CurRom->BroadcastRoom( "%s*** %s has died! ***%s\n\r",
		ANSI_BR_CYAN, Player->Player.GetFirstName(), ANSI_WHITE );

	Rom->BroadcastRoom( "%s%s appears in a flash of light!%s\n\r",
		ANSI_BR_RED, Player->Player.GetFirstName(), ANSI_WHITE );
	
	Rom->AddPlayerToRoom( Player );
	
	Player->Player.SetRoom( Rom );
	
	WriteToBuffer( Player, "%s*** You have died. ***%s\n\r", 
		ANSI_BR_CYAN, ANSI_WHITE );
	
	Player->Player.SubtractKill();
	return;
}

/*======================================================================
DisplayCombatStatus()

Simple way to display combat status

Status is a boolean true or false.  (is combat true, or false)
======================================================================*/

void DisplayCombatStatus(Connection *Player, bool Status)
{
	Room *CurRoom = Player->Player.GetRoom();
	Connection *Victim = Player->Victim;

	if (Player->Victim == '\0')
	{
		ServerLog( "DisplayCombatStatus: Player had no victim! ...bailing out of function!");
		return;
	}
	switch(Status)
	{
	case true:
 		WriteToBuffer( Player, "%s*** Combat Engaged ***%s\n\r", ANSI_YELLOW, ANSI_WHITE );
		WriteToBuffer( Player->Victim, "%s*** %s moves to attack you! ***%s\n\r",ANSI_YELLOW, Player->Player.GetFirstName(), ANSI_WHITE );
		CurRoom->SelectiveBroadcast( Player, Victim, "%s%s moves to attack %s!%s\n\r", ANSI_YELLOW, 
			Player->Player.GetFirstName(), Victim->Player.GetFirstName(), ANSI_WHITE );
		break;
	case false:
		WriteToBuffer( Player, "%s*** Combat Off ***%s\n\r", ANSI_YELLOW, ANSI_WHITE );
		break;
	}
}

/*======================================================================
DisplayMeleeCombat()

Simple way to display combat attacks

======================================================================*/

void DisplayMeleeCombat(Connection *Player, char *attackType, char *Weapon, int Damage, bool Critical)
{
	Room *CurRoom = Player->Player.GetRoom();
	if (Player->Victim == '\0')
		ServerLog( "DisplayCombat: Player had no victim! ...bailing out of function!");

	if (Damage == 0) {
		// Tell the attacker
		WriteToBuffer( Player, "%sYou swing at %s with your %s, but miss!%s\n\r",
			ANSI_BR_RED, Player->Victim->Player.GetFirstName(),
		    Weapon, ANSI_WHITE );

		// Tell the victim
		WriteToBuffer( Player->Victim, "%s%s swings at you with his %s, but missed!%s\n\r",
			ANSI_BR_RED, Player->Player.GetFirstName(), Weapon, ANSI_WHITE );

		// Let everyone else in the room know that the attacker is blind as a bat!
		CurRoom->SelectiveBroadcast( Player, Player->Victim, "%s%s swings at %s with his %s, but misses!%s\n\r",
			ANSI_BR_RED, Player->Player.GetFirstName(), Player->Victim->Player.GetFirstName(), Weapon, ANSI_WHITE );
	}
	else if (!Critical) 
	{
		// Tell the attacker
		WriteToBuffer( Player, "%sYou %s %s with your %s for %d damage!%s\n\r",
			ANSI_BR_RED, attackType, Player->Victim->Player.GetFirstName(),
		    Weapon, Damage, ANSI_WHITE );

		// Tell the victim
		WriteToBuffer( Player->Victim, "%s%s %ss you with his %s for %d damage!%s\n\r",
			ANSI_BR_RED, Player->Player.GetFirstName(), attackType,
		    Weapon, Damage, ANSI_WHITE );

		// Let everyone else in the room know the victim got punked!
		CurRoom->SelectiveBroadcast( Player, Player->Victim, "%s%s %ss %s with his %s for %d damage!%s\n\r",
			ANSI_BR_RED, Player->Player.GetFirstName(), attackType,
			Player->Victim->Player.GetFirstName(), Weapon,
			Damage, ANSI_WHITE );
	}
	else
	{
		// Tell the attacker
		WriteToBuffer( Player, "%sYou critically %s %s with your %s for %d damage!%s\n\r",
			ANSI_BR_RED, attackType, Player->Victim->Player.GetFirstName(),
		    Weapon, Damage, ANSI_WHITE );

		// Tell the victim
		WriteToBuffer( Player->Victim, "%s%s critically %ss you with his %s for %d damage!%s\n\r",
			ANSI_BR_RED, Player->Player.GetFirstName(), attackType,
		    Weapon, Damage, ANSI_WHITE );

		// Let everyone else in the room know the victim got punked!
		CurRoom->SelectiveBroadcast( Player, Player->Victim, "%s%s critically %ss %s with his %s for %d damage!%s\n\r",
			ANSI_BR_RED, Player->Player.GetFirstName(), attackType,
			Player->Victim->Player.GetFirstName(), Weapon,
			Damage, ANSI_WHITE );
	}


}

/*======================================================================
DisplayStunStatus()

Simple way to display stun output

======================================================================*/

void DisplayStunStatus( Connection *Player )
{
	Room *CurRoom = Player->Player.GetRoom();
    // Tell the victim 
	WriteToBuffer( Player, "%sYou are stun!%s\n\r",
		ANSI_BLUE, ANSI_WHITE );

	// Tell the room */
	CurRoom->SelectiveBroadcast( Player, Player, "%s%s stumbles backwards!%s\n\r",
		ANSI_BLUE, Player->Player.GetFirstName(), ANSI_WHITE );
}

/*======================================================================
StopCombat()

Simple way cleanly shutdown combat

======================================================================*/
void StopCombat( Connection *Player )
{
	Player->Victim = '\0';

	// Kill any attack event in the queue!
	if (Player->Player.GetAttackEvent())
	{	
		Player->Player.GetAttackEvent()->killEvent(NULL);
		Player->Player.SetAttackEvent('\0');
	}
}

