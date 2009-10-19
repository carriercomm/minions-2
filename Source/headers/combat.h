/***************************************************************
 *           Minions Mud Server is copyright (C)2001           *
 *                    SonzoSoft Software                       *
 *                                                             *
 *         Programmed by Mark Richardson and David Brown       *
 *															   *
 *			 Mark Richardson	-	sinbaud@hotmail.com	       *
 *		       David Brown	    -	dcbrown73@yahoo.com	       *
 ***************************************************************/
#if !defined _COMBAT_H_INCLUDED_
#define _COMBAT_H_INCLUDED_

struct Connection; //forward declaration
class Room;			//forward declaration

/*  Different types of possible attacks  */
enum AttackType
{
	NORMAL_SPELL, AREA_SPELL, UNARMED_MELEE, ARMED_MELEE
};

/* this list contains players that are actively attacking someone */
struct CombatList
{
	Connection		*Attacker;
	Connection		*Victim;
	AttackType		TypeOfAttack;	
	CombatList		*Next;
};

/*  function prototypes for comabt.cpp module */
void DoCombatRound( void );
void Die( Connection *Player, Room *CurRom );
void AddToCombatList( Connection *Agressor, Connection *Victim, AttackType AType );
void RemoveFromCombatList( Connection *Aggressor, Connection *Victim );


#endif //_COMBAT_H_INCLUDED