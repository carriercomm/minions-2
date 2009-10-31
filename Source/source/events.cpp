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


using namespace std;


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