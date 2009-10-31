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
scheduler -> pushExecStack()

Add event object on to the execStack.
===============================================================*/

//void scheduler::pushExecStack(time_t eTime, minionsEvent *mEvent)
//{
//	esIter=execStack.end();
//	execStack.insert(esIter, pair<time_t, minionsEvent *>(eTime, mEvent));
//};

/*==============================================================
scheduler -> doEvents()

Take execStack and execute the events then delete the event and multimap element
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
scheduler -> checkEventsStatus()

Check the exec times in the waitStack and apply the events
ready to be executed into the execStack
===============================================================*/

void scheduler::checkEventsStatus()
{
	time_t currentTime;
	unsigned int stackSize;
	int x;
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
	//for (thisEvent=tmpStack.begin(); thisEvent != tmpStack.end(); thisEvent++)
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