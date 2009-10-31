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
#include "combat.h"
#include "scheduler.h"
#include "tcpcomm.h"
#include "player.h"


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
}

// meCombat class Destructor
meCombat::~meCombat() { };

/*===============================================================
meCombat -> execCombat()

Execute combat round
===============================================================*/
void meCombat::execEvent(scheduler *eventScheduler)
{
    DoCombatRound();
	minionsEvent *combatEvent = new meCombat();
	eventScheduler->pushWaitStack((time(NULL) + COMBAT_TIME_INTERVAL), combatEvent);
}















/*==============================================================
 meHeal class -> Derived from minionsEvent class

 Constructor:

 Healing event to schedule regular healing sequence.
===============================================================*/
meHeal::meHeal()
{
	eventType = ME_HEAL;
	eventTime = (time(NULL) + HEAL_TIME_INTERVAL);
}

// meCombat class Destructor
meHeal::~meHeal() { };

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
	int healRate;
/*
   Need to add "atRest" player stats.  If a player is resting, he heals at 2x rest rate.
   For now, I'm using player level as his heal rate.  1st level heals at 1hp per tick. 
   Maybe if they are resting, they heal at healRate times 2?
*/


	for( TempConn = PlayerList; TempConn; TempConn = TempConn->Next )
	{
		curHP=TempConn->Player.GetHitPoints();
		maxHP=TempConn->Player.GetMaxHitPoints();
		// Setting heal rate to the players level. (probably should look into this later)
		healRate=TempConn->Player.GetLevel();

		// Is the player at max hit points?
		if (curHP < maxHP)
		{
			// Nope, lets see if one heal would raise the player above max hitpoints
			// if it does, set maximum HPs, otherwise heal player at healRate
			if ((curHP + healRate) > maxHP)
			{
				TempConn->Player.SetHitPoints(maxHP);
			}
			else
			{
				TempConn->Player.SetHitPoints((curHP + healRate));
			}
		}
	}
};