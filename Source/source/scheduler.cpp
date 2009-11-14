/***************************************************************
 *           Minions Mud Server is copyright (C)2001           *
 *                    SonzoSoft Software                       *
 *                                                             *
 *         Programmed by Mark Richardson and David Brown       *
 *															   *
 *			 Mark Richardson	-	sinbaud@hotmail.com	       *
 *		       David Brown	    -	dcbrown73@yahoo.com	       *
 ***************************************************************/

#include <iostream>
#include <map>
#include <vector>
#include "time.h"
#include "events.h"
#include "scheduler.h"
#include "tcpcomm.h"
#include "combat.h"
//#include "erase.h"

using namespace std;
//using namespace detail;


/*==============================================================
scheduler class contructor and destructor
==============================================================*/
scheduler::scheduler() { };
scheduler::~scheduler() { };

/*==============================================================
scheduler -> getWaitStack()

This functions returns a copy of the entire waitStack and
then zeros out the waitStack preparing it for new events
to be placed in it by the primary game loop
==============================================================*/

scheduler::wStack scheduler::getWaitStack()
{
	wStack tmpStack;

	tmpStack = waitStack;
	waitStack.clear();
	return tmpStack;
}


/*==============================================================
scheduler -> getExecStack()

This functions returns a copy of the entire waitStack and
then zeros out the waitStack preparing it for new events
to be placed in it by the primary game loop
==============================================================*/

scheduler::eStack scheduler::getExecStack()
{
	eStack tmpStack;
                         
	tmpStack = execStack;
	execStack.clear();
	return tmpStack;
}


/*==============================================================
scheduler -> pushWaitStack()

Add event object on to the waitStack.
===============================================================*/

void scheduler::pushWaitStack(time_t eTime, minionsEvent *mEvent)
{
	wsIter=waitStack.end();
	waitStack.insert(wsIter, pair<time_t, minionsEvent *>(eTime, mEvent));
};


/*==============================================================
scheduler -> pushCombatStack()

Add event object on to the combatStack.
===============================================================*/

void scheduler::pushCombatStack(minionsEvent *mEvent)
{
	combatStack.push_back(mEvent);
};


/*==============================================================
scheduler -> doEvents()

Take execStack and execute the events then delete the event and vector element
===============================================================*/

void scheduler::doEvents(scheduler *eventScheduler)
{
	minionsEvent *p;
	eStack::iterator curEvent;

	for (curEvent=execStack.begin(); curEvent != execStack.end(); ++curEvent) 
	{
		(*curEvent)->execEvent(eventScheduler);
		p=*curEvent;
		delete p;
	}
	execStack.clear();
};


/*==============================================================
scheduler -> doMeleeEvents()

Take execStack and execute the events then delete the event and vector element
===============================================================*/

void scheduler::doMeleeEvents(scheduler *eventScheduler)
{
	minionsEvent *p;
	eStack::iterator curEvent;
	eStack tmpStack;

	tmpStack.swap(combatStack);
	combatStack.clear();

	for (curEvent=tmpStack.begin(); curEvent != tmpStack.end(); ++curEvent) 
	{
		(*curEvent)->execEvent(eventScheduler);
		p=*curEvent;
		delete p;
	}
};

/*==============================================================
scheduler -> checkEventsStatus()

Check the exec times in the waitStack and apply the events
ready to be executed into the execStack
===============================================================*/

void scheduler::checkEventsStatus()
{
	time_t currentTime;
	size_t stackSize;
	size_t x;
	wStack tmpStack;
	wStack::iterator thisEvent;

    // Swap waitStack and tmpStack so I can move events around without
	// screwing up the iterator with erasing elements
	tmpStack.swap(waitStack);
	currentTime=time(NULL);
    
	stackSize=tmpStack.size();
	x=0;
	thisEvent=tmpStack.begin();
	// Loops through all events that are ready to be executed
	for (x=1; x <= stackSize; x++)
	{
		if (currentTime > (*thisEvent).first) 
		{
		   execStack.push_back(((*thisEvent).second));
		}
	    else {
			pushWaitStack((*thisEvent).first, (*thisEvent).second);
        }
		thisEvent++;
	}
};


void scheduler::ClearPlayerEvents( Connection *Conn )
{
	Connection *Attacker;
	// Iterator for stack
	eStack::iterator curEvent;
	// Melee Specific point so we can access extra functions not
	// accessable through the generic minionsEvent pointer.
	minionsEvent *Event;

	// Kill all Melee events for both the disconnected and those attacking the disconnected player
	for (curEvent=combatStack.begin(); curEvent != combatStack.end(); ++curEvent)
	{
		Event=*curEvent;
		if (Event->getVictim() == Conn )
		{
			Attacker = Event->getAttacker();
			DisplayCombatStatus(Attacker, false);
			StopCombat(Event->getAttacker());
		}
		else if ( Event->getAttacker() == Conn )
			StopCombat(Conn);
	}
}