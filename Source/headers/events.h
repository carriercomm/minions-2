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
const int COMBAT_TIME_INTERVAL      =   4;
const int HEAL_TIME_INTERVAL        =  15;
const int STUN_TIME_INTERVAL        =   4;


// Define Minion event types
const int ME_COMBAT                 =   1;
const int ME_HEAL                   =   2; 
const int ME_MELEE_COMBAT           =   3;
const int ME_REMOVE_FLAG            =   4;
const int ME_TIME_SPELL             =   5;
const int ME_COMBAT_SPELL           =   6;

// Combat stuff 

const int CRITICAL                  =  89;
const int MAX_PUNCH_DAMAGE          =   5;
const int MIN_PUNCH_DAMAGE          =   1;
const int LUCK_ROLL                 =  97;

// Rates
const int HEAL_RATE                 =   3;

// enum of flags
enum Flags {
		E_FLAG_HIDDEN,
		E_FLAG_FIGHTING,
		E_FLAG_BLIND,
		E_FLAG_SNEAKING,
		E_FLAG_GOSSIPS,
		E_FLAG_TELEPATHS,
		E_FLAG_HIDING,
		E_FLAG_STUN
}; 	

// Forward declarations
class  scheduler;
struct Connection;
class  Room;
class  Client;
class  Item;
class  MeleeSpell;
class  BaseTimeSpell;

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
	bool deadEvent;
	Connection *Player;
	Connection *Victim;

public:

	int getEventObjectType();
	time_t getEventTime();
	virtual void execEvent(scheduler *eventScheduler)=0;  // Virtual functions def =0 marks it as purely virtual 
	virtual void killEvent(Connection *Conn)=0;
	virtual Connection *getAttacker()=0;
	virtual Connection *getVictim()=0;

};


/*==============================================================
 meCombat class -> Derived from minionsEvent class

 Combat event to schedule running the common combat sequence.
===============================================================*/
class meCombat : public minionsEvent
{
//	Connection *Player;
//	Connection *Victim;
public:
    meCombat();
	~meCombat();
	void execEvent(scheduler *eventScheduler);
	void killEvent(Connection *Conn);
	Connection *getAttacker() { return Player; };
	Connection *getVictim() { return Victim; };
};


/*==============================================================
 meHeal class -> Derived from minionsEvent class

 Heal event to schedule normal timed healing cycle
===============================================================*/
class meHeal : public minionsEvent
{
//	Connection *Player;
//	Connection *Victim;
public:
    meHeal();
	~meHeal();
	void execEvent(scheduler *eventScheduler);
	void killEvent(Connection *Conn);
	void naturalHeal();
	Connection *getAttacker() { return Player; };
	Connection *getVictim() { return Victim; };
};


/*==============================================================
 meMelee class -> Derived from minionsEvent class

 Melee event, all melee (weapn) based attacks that come in 
 the timed combat round.
===============================================================*/

class meMelee : public minionsEvent
{
//	Connection *Player;
//	Connection *Victim;
	Item *Weapon;

public:

	char punch[6];
	char punchWeapon[5];
	meMelee(Connection *Attacker, Connection *Attacked);
	~meMelee();
	void execEvent(scheduler *eventScheduler);
	void killEvent(Connection *Conn);
	Connection *getAttacker() { return Player; };
	Connection *getVictim() { return Victim; };
};

/*==========================================================
 meRemoveFlag class -> Derived from minionsEvent class

Removes any flags set when time runs out


===========================================================*/
class meRemoveFlag : public minionsEvent
{
//	Connection *Player;
//	Connection *Victim;
	int Flag;

public:
    meRemoveFlag(Connection *Player, int FlagToSet);
	~meRemoveFlag();
	void execEvent(scheduler *eventScheduler);
	void killEvent(Connection *Conn);
	Connection *getAttacker() { return Player; };
	Connection *getVictim() { return Victim; };
};



/*==========================================================
 meCombatSpell class -> Derived from minionsEvent class

Spell events class.  Spell objects are applied to this event
===========================================================*/

class meCombatSpell : public minionsEvent
{
	MeleeSpell *SpellCasted;

public:

	meCombatSpell(Connection *Attacker, Connection *Attacked, MeleeSpell *Spell);
	~meCombatSpell();
	void execEvent(scheduler *eventScheduler);
	void killEvent(Connection *Conn);
	Connection *getAttacker() { return Player; };
	Connection *getVictim() { return Victim; };
};




/*==============================================================
 meTimeSpell class -> Derived from minionsEvent class

 Time based spell event.  This is the scheduled event with a spell
 object attached.  This DOES NOT point to a singluler spell object.
 It holds it's OWN object.
===============================================================*/
class meTimeSpellEvent : public minionsEvent
{
	//Connection        *Player;
	//Connection        *Victim;
	BaseTimeSpell     *SpellCasted;
	int                SpellStage;

public:
    meTimeSpellEvent( Connection *Attacker, Connection *Attacked, BaseTimeSpell *Spell, int RepeatTimes );
	~meTimeSpellEvent();
	void execEvent(scheduler *eventScheduler);
	void killEvent(Connection *Conn);
	Connection *getAttacker() { return Player; };
	Connection *getVictim() { return Victim; };
};
