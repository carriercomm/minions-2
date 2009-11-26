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
extern scheduler    eventScheduler;


/**********************************************************
 *   Functions                                            *
 **********************************************************/


/**********************************************************
 Die()

 Called when some poor sap bites the big one           *
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
	// Kill all combat events envolving the deceased
	eventScheduler.ClearPlayerEvents( Player );

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

	if (Player->Victim == NULL)
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
		WriteToBuffer( Player, "%sYou swing at %s with your %s, but miss!%s\n\r", ANSI_WHITE,
			Player->Victim->Player.GetFirstName(), Weapon, ANSI_WHITE );

		// Tell the victim
		WriteToBuffer( Player->Victim, "%s%s swings at you with his %s, but missed!%s\n\r", ANSI_WHITE, 
			Player->Player.GetFirstName(), Weapon, ANSI_WHITE );

		// Let everyone else in the room know that the attacker is blind as a bat!
		CurRoom->SelectiveBroadcast( Player, Player->Victim, "%s%s swings at %s with his %s, but misses!%s\n\r", 
			ANSI_WHITE, Player->Player.GetFirstName(), Player->Victim->Player.GetFirstName(), Weapon, ANSI_WHITE );
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
	Player->Victim = NULL;

	// Kill any attack event in the queue!
	if (Player->Player.GetAttackEvent())
	{	
		Player->Player.GetAttackEvent()->killEvent(NULL);
		Player->Player.SetAttackEvent(NULL);
	}
}


/*======================================================================
SlipAndFall()

Player slips and false. (breaks combat)

======================================================================*/

void SlipAndFall( Connection *Player )
{
	Room *CurRoom = Player->Player.GetRoom();

	// Tell the attacker
	WriteToBuffer( Player, "%sYou lunge wildly at %s, but miss losing your balance crashing to the floor!%s\n\r",
		ANSI_BR_YELLOW, Player->Victim->Player.GetFirstName(), ANSI_WHITE );

	// Tell the victim
	WriteToBuffer( Player->Victim, "%s%s lunges wildly at you, but misses losing his balance and crashing to the floor!%s\n\r",
		ANSI_BR_YELLOW, Player->Player.GetFirstName(), ANSI_WHITE );

	// Let everyone else in the room know the victim got punked!
	CurRoom->SelectiveBroadcast( Player, Player->Victim, "%s%s lunges wildly at %s, but misses losing his balance and crashing to the floor!%s\n\r",
		ANSI_BR_YELLOW, Player->Player.GetFirstName(),	Player->Victim->Player.GetFirstName(), ANSI_WHITE );
	
	// Can't attack attack while on the ground!
	DisplayCombatStatus( Player, false );
	StopCombat( Player );
	
}


/*======================================================================
LoseWeapon()

Player's weapon slips out of his hand during attack

======================================================================*/

void LoseWeapon( Connection *Player )
{
	Room *CurRoom = Player->Player.GetRoom();

	// Tell the attacker
	WriteToBuffer( Player, "%sYou lose control of your %s as it flies out of your hand!%s\n\r",
		ANSI_BR_YELLOW, Player->Player.GetWieldedItem()->GetItemName(), ANSI_WHITE );

	// Tell the victim
//	WriteToBuffer( Player->Victim, "%s%s loses control of his weapon as it flies out hif his hand!s's %s flies!%s\n\r",
//		ANSI_BR_YELLOW, Player->Player.GetFirstName(), ANSI_WHITE );

	// Let everyone else in the room know the victim got punked!
	CurRoom->SelectiveBroadcast( Player, Player, "%s%s loses control of his %s as it flies out his his hand!%s\n\r",
		ANSI_BR_YELLOW, Player->Player.GetFirstName(), Player->Player.GetWieldedItem()->GetItemName(), ANSI_WHITE );
	
	// Can't attack attack while on the ground!
	CurRoom->AddItemToRoom( Player->Player.GetWieldedItem() );
	Player->Player.WearItem( NULL, Player->Player.GetWieldedItem()->GetWearLocation() );
	DisplayCombatStatus( Player, false );
	StopCombat( Player );
	
}