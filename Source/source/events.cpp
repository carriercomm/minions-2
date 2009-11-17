/***************************************************************
 *           Minions Mud Server is copyright (C)2001           *
 *                    SonzoSoft Software                       *
 *                                                             *
 *         Programmed by Mark Richardson and David Brown       *
 *															   *
 *			 Mark Richardson	-	sinbaud@hotmail.com	       *
 *		       David Brown	    -	dcbrown73@yahoo.com	       *
 ***************************************************************/
#include <time.h>
#include <iostream>
#include <map>
#include <vector>
#include "events.h"
#include "room.h"
#include "item.h"
#include "combat.h"
#include "scheduler.h"
#include "tcpcomm.h"
#include "player.h"
#include "commands.h"
#include "ansicolor.h"


using namespace std;

// Globals
extern Connection	*PlayerList;


//==============================================================
// minionsEvent class
//
// This is the base class for scheduled events in Minions. 
// 
// Each even that requires different parms, will have it's
// own event type class object derived from this base class.
// Using this class as a base, allows us to determine it's
// type by using the eventType varible and calling 
// obj->getEventObjectType(); which will allow us to call
// the correct functions for that object type allowing
// proper execution. It also holds the execution time. (eventTime)
//==============================================================

//minionsEvent::minionsEvent() { };

/*==============================================================
getEventObjectType()

Get and return the object type identifier
==============================================================*/

int minionsEvent::getEventObjectType()
{
	return eventType;
};

/*==============================================================
getEventTime()

Return eventTime (time which the event should be executed)
==============================================================*/
time_t minionsEvent::getEventTime()
{
	return eventTime;
};













/*==============================================================
 meCombat class -> Derived from minionsEvent class

 Constructor:

 Combat event to schedule running the common combat sequence.
===============================================================*/
meCombat::meCombat()
{
	eventType = ME_COMBAT;
	eventTime = (time(NULL) + COMBAT_TIME_INTERVAL);
	deadEvent = false;
}

// meCombat class Destructor
meCombat::~meCombat() { };


/*===============================================================
meCombat -> execCombat()

Execute combat round
===============================================================*/
void meCombat::execEvent(scheduler *eventScheduler)
{
	eventScheduler->doMeleeEvents(eventScheduler);
	//DoCombatRound();
	minionsEvent *combatEvent = new meCombat();
	eventScheduler->pushWaitStack((time(NULL) + COMBAT_TIME_INTERVAL), combatEvent);
};

void meCombat::killEvent(Connection *Conn)
{
	deadEvent = true;
};











/*==============================================================
 meHeal class -> Derived from minionsEvent class

 Constructor:

 Healing event to schedule regular healing sequence.
===============================================================*/
meHeal::meHeal()
{
	eventType = ME_HEAL;
	eventTime = (time(NULL) + HEAL_TIME_INTERVAL);
	deadEvent = false;
}

// meCombat class Destructor
meHeal::~meHeal() { };

void meHeal::killEvent(Connection *Conn)
{
	deadEvent = true;
};

/*===============================================================
meHeal -> execEvent()

Execute healing event and add next healing event to stack
===============================================================*/
void meHeal::execEvent(scheduler *eventScheduler)
{
    naturalHeal();
	minionsEvent *healEvent = new meHeal();
	eventScheduler->pushWaitStack((time(NULL) + HEAL_TIME_INTERVAL), healEvent);
}

/*===============================================================
meHeal -> naturalHeal()

This is the function that runs through all players and gives their
natural healing at the set healing interval and their heal rate
===============================================================*/

void meHeal::naturalHeal()
{
	Connection  *TempConn = '\0';
	int curHP;
	int maxHP;
	int healRate = HEAL_RATE;
/*
   If a player is resting, he heals at 2x rest rate.
   For now, I'm using player level as his heal rate.  1st level heals at 1hp per tick. 
   Maybe if they are resting, they heal at healRate times 2?
*/


	for( TempConn = PlayerList; TempConn; TempConn = TempConn->Next )
	{
		curHP=TempConn->Player.GetHitPoints();
		maxHP=TempConn->Player.GetMaxHitPoints();
		// Setting heal rate to the players level. (probably should look into this later)
		

		if (TempConn->Player.GetRestingStatus() == RESTING)
		{
			healRate *= 2;
		}
		// Is the player at max hit points?
		if (curHP < maxHP)
		{
			// Nope, lets see if one heal would raise the player above max hitpoints
			// if it does, set maximum HPs, otherwise heal player at healRate
			if ((curHP + healRate) > maxHP)
				TempConn->Player.SetHitPoints(maxHP);
			else
				TempConn->Player.SetHitPoints((curHP + healRate));
			WriteToBuffer( TempConn, "%s%s[ HP: %i] > %s", ANSI_CLR_SOL, ANSI_BR_CYAN, TempConn->Player.GetHitPoints(), ANSI_WHITE );

		}
	}
};




/*==============================================================
 meMelee class -> Derived from minionsEvent class

 Constructor:

 Melee combat event for physcial attacks iwth a weapons (or fist)
===============================================================*/

meMelee::meMelee(Connection *Attacker, Connection *Attacked)
{
	eventType          = ME_MELEE_COMBAT;
	eventTime          = (time(NULL));
	deadEvent          = false;
	Player             = Attacker;
	Victim             = Attacked;
	Weapon             = '\0';
	Player->Victim     = Attacked;
};

/*===============================================================
killEvent -> meMelee

This kills events in the event queue.  It takes one argument of 
type connection.  If the connection is NULL, just kill the event.
If it has an actual address location, compare Player->Victim to
the Conn pointer.  If they are the same, kill the event.  

killEvent is called with a pointer (not null) if a player disconnect
This allows us to delete any associated objects for the disconnected
player
================================================================*/
void meMelee::killEvent(Connection *Conn)
{
	// Null means just kill the event. 
	if (Conn == NULL)
		deadEvent = true;
	else
	{   // If Conn == Victim, kill the event as the Victim disconnected
		// If it doesn't, nothing to do.
		if (Conn == Player->Victim )
			deadEvent = true;
	}
};

/*===============================================================
execEvent -> meMelee

Execute combat event and add next combat event to stack if victim still alive
===============================================================*/
void meMelee::execEvent(scheduler *eventScheduler)
{
	int			Damage = 0;
	int			ToHitRoll = 0;
	int			ToHitValue = 0;
	int			AC = 0;
	int			Thac0 = 0;
	char        *attackType;
	char        *theWeapon;
	char        punch[]       = "punch";
	char        punchWeapon[] = "fist";
	bool        Critical = false;
	Room        *CurRoom = Player->Player.GetRoom();
	Item        *Weapon = Player->Player.GetWieldedItem();

	// Is this a dead event?  If so, skip it!
	if (deadEvent == true) {
		return;
	}
	// Is victim already dead?
	if (!Player->Victim)
		return;

	// Is the victim in the room?  If not, close combat sequence
	if (CurRoom != Player->Victim->Player.GetRoom())
	{
		DisplayCombatStatus(Player, false);
		Player->Victim = NULL;
		return;
	}

	// All appears good, lets kill a mofo!
    CurRoom = Player->Player.GetRoom();
	AC = Player->Player.GetArmorClass();
	Thac0 = Player->Player.GetTHAC0();
	ToHitValue = Thac0 - AC;
	ToHitRoll = rand() % 20 + 1;

	
	// Set attack type (could be punch)
	if (!Weapon)
	{
		attackType = punch;
		theWeapon = punchWeapon;
	}
	else
		attackType = Weapon->GetAttackType();
	

	// Did the player HIT and CRIT?
	if ((ToHitRoll > ToHitValue) && (ToHitRoll > CRITICAL))
	{
		Critical = true;
		if (!Weapon) // Does he have a weapon or is he punching?
			Damage = (rand() % MAX_PUNCH_DAMAGE) + (MAX_PUNCH_DAMAGE + 1);
		else 
			Damage = ( rand() % Weapon->GetMaxDamage() ) + (Weapon->GetMaxDamage() + 1);
	}
	else if (ToHitRoll > ToHitValue) // Ok, did he at least hit the player?
	{
		if (!Weapon)
			Damage = (rand() % MAX_PUNCH_DAMAGE);
		else
			Damage = (rand() % Weapon->GetMaxDamage());
	}

	// If damage is greater than 0 and a critical, that means someone got punked!
	if (Damage > 0)
	{
		if(!Weapon)
			DisplayMeleeCombat(Player, attackType, theWeapon, Damage, Critical);
		else
			DisplayMeleeCombat(Player, attackType, Weapon->GetItemName(), Damage, Critical);
	}
	else
	{
		if(!Weapon)
			DisplayMeleeCombat(Player, attackType, theWeapon, Damage, Critical);
		else
			DisplayMeleeCombat(Player, attackType, Weapon->GetItemName(), Damage, Critical);
	}
	//Did we stun him?
	if ( ToHitRoll == 20 )
	{	
		DisplayStunStatus( Victim );
		SET_PFLAG(Victim->Flags, FLAG_STUN);
		minionsEvent *RemoveEvent = new meRemoveFlag(Victim, E_FLAG_STUN);
		eventScheduler->pushWaitStack((time(NULL)+STUN_TIME_INTERVAL), RemoveEvent);
		StopCombat(Victim);

	}

	// Is the player dead?  If so, kill his ass and close combat
	if(!Player->Victim->Player.UpdateHitPoints( Damage, false ))
	{ 	
		Die( Victim, CurRoom );
		DisplayCombatStatus(Player, false);
		Player->Player.AddKill();
		return;
	}

	// Not dead, so keep attacking (add another combat event)
	minionsEvent *meleeEvent = new meMelee(Player, Victim);
	// Set pointer to new attack event in case it needs to be cancelled
    Player->Player.SetAttackEvent(meleeEvent);
	// Add new event to combatStack
	eventScheduler->pushCombatStack(meleeEvent);
}


	

/*========================================================================
meRemoveFlag

=========================================================================*/

meRemoveFlag::meRemoveFlag(Connection *StunPlayer, int FlagToSet)
{
	eventType = ME_REMOVE_FLAG;
	eventTime = (time(NULL) + STUN_TIME_INTERVAL);
	Player = StunPlayer;
	Victim = StunPlayer;
	deadEvent = false;
	Flag = FlagToSet;
}

// meCombat class Destructor
meRemoveFlag::~meRemoveFlag() { };

void meRemoveFlag::killEvent(Connection *Conn)
{
	deadEvent = true;
};

/*===============================================================
meRemoveFlag -> execEvent()

Execute healing event and add next healing event to stack
===============================================================*/
void meRemoveFlag::execEvent(scheduler *eventScheduler)
{
	switch (Flag)
	{
	case E_FLAG_HIDDEN:
		break;
	case E_FLAG_FIGHTING:
		break;
	case E_FLAG_BLIND:
		break;
	case E_FLAG_SNEAKING:
		break;
	case E_FLAG_GOSSIPS:
		break;
	case E_FLAG_TELEPATHS:
		break;
	case E_FLAG_HIDING:
		break;
	case E_FLAG_STUN:
		REMOVE_PFLAG(Player->Flags, FLAG_STUN);
		WriteToBuffer(Player, "%sYou are no longer stun.\n\r%s", ANSI_BLUE, ANSI_WHITE);
		break;
	default:
		break;
	}
}
