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
#include "combat.h"
#include "room.h"
#include "ansicolor.h"
#include "tcpcomm.h"
#include "item.h"

/**********************************************************
 *   Global Variables                                     *
 **********************************************************/
extern Connection	*PlayerList;
CombatList			*FirstCombantant = '\0';


/**********************************************************
 *   Functions                                            *
 **********************************************************/
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
	Player->Victim = '\0';

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
