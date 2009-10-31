/***************************************************************
 *           Minions Mud Server is copyright (C)2001           *
 *                    SonzoSoft Software                       *
 *                                                             *
 *         Programmed by Mark Richardson and David Brown       *
 *															   *
 *			 Mark Richardson	-	sinbaud@hotmail.com	       *
 *		       David Brown	    -	dcbrown73@yahoo.com	       *
 ***************************************************************/


// Define Combat time interval in seconds
#define COMBAT_TIME_INTERVAL            4
#define HEAL_TIME_INTERVAL             15


// Define Minion event types
#define ME_COMBAT                       1
#define ME_HEAL                         2 


class scheduler;

/*==============================================================
minionsEvent class

This is the base class for scheduled events in Minions. 
 
Each event that requires different parms, will have it's
own event type class object derived from this base class.
Using this class as a base, allows us to determine its
type by using the eventType varible and calling 
obj->getEventObjectType(); which will allow us to call
the correct functions for that object type allowing
proper execution. It also holds the execution time. (eventTime)

Note: For consistency, all derived classes should start 
with "me" (M)inions (E)vent.  (ie, meCombat)

===============================================================*/

class minionsEvent
{
protected:
	int eventType;
	time_t eventTime;

public:

	int getEventObjectType();
	time_t getEventTime();
	virtual void execEvent(scheduler *eventScheduler)=0;  // Virtual functions def =0 marks it as purely virtual 

};


/*==============================================================
 meCombat class -> Derived from minionsEvent class

 Combat event to schedule running the common combat sequence.
===============================================================*/
class meCombat : public minionsEvent
{
public:
    meCombat();
	~meCombat();
	void execEvent(scheduler *eventScheduler);
};


/*==============================================================
 meHeal class -> Derived from minionsEvent class

 Heal event to schedule normal timed healing cycle
===============================================================*/
class meHeal : public minionsEvent
{
public:
    meHeal();
	~meHeal();
	void execEvent(scheduler *eventScheduler);
	void naturalHeal();
};