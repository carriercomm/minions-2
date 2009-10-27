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
#include "time.h"
#include "events.h"
#include "scheduler.h"

using namespace std;



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

scheduler::eventStack scheduler::getWaitStack()
{
	eventStack tmpStack;

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

scheduler::eventStack scheduler::getExecStack()
{
	eventStack tmpStack;

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

void scheduler::pushExecStack(time_t eTime, minionsEvent *mEvent)
{
	esIter=execStack.end();
	waitStack.insert(esIter, pair<time_t, minionsEvent *>(eTime, mEvent));
};