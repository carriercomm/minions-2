/***************************************************************
 *           Minions Mud Server is copyright (C)2001           *
 *                    SonzoSoft Software                       *
 *                                                             *
 *         Programmed by Mark Richardson and David Brown       *
 *															   *
 *			 Mark Richardson	-	sinbaud@hotmail.com	       *
 *		       David Brown	    -	dcbrown73@yahoo.com	       *
 ***************************************************************/
#include <map>
#include <string>

struct Connection;
class minionsEvent;

#define MELEE_SPELLS_DATABASE     "minions.db"


// Which spell effect text do you want?
// BaseTimeSpell::GetSpellText(which_text)
const int INIT_SPELL_TEXT      =    0;
const int SPELL_EFFECT_TEXT    =    1;
const int END_EFFECT_TEXT      =    2;


const int NO_SPELL             =    0;
const int MAGE1                =    1;
const int MAGE2                =    2;
const int MAGE3                =    3;
const int PRIEST1              =    4;
const int PRIEST2              =    5;
const int PRIEST3              =    6;
const int DRUID1               =    7;
const int DRUID2               =    8;
const int DRUID3               =    9;
const int NECRO1               =   10;
const int NECRO2               =   11;
const int NECRO3               =   12;

//using std::string;
using namespace std;



/*==============================================================
MeleeSpell class

This is the Spell base class.  All spells we derive from this 
class
===============================================================*/

class MeleeSpell
{
protected:
	string			   SpellName;          // Name of spell
	string			   SpellDescription;
	string			   SpellCommand;       // 4 letter command "mmis" for magic missle
	string			   SpellString;        // Probably not going to use this. (smash, slam, etc)
	int			       SpellNumber;
	int				   SpellType;          // define (DoT, HoT, Curse, RemoveCurse, Heal, etc)
	int				   SpellClass;         // Mage1, Mage2, Mage3, Priest1, Priest2, etc
	int				   ManaCost;           // All spells cost mana!
	int                AttackCount;
	int				   MinDamage;
	int                MaxDamage;




public:
	// Constructor / destrucutor

	// Get values
	string		   GetSpellName( void )                     { return SpellName; };
	string		   GetSpellDescription ( void )             { return SpellDescription; };
	string		   GetSpellCommand( void )                  { return SpellCommand; };
	int            GetSpellType( void )                     { return SpellType; };
	int            GetSpellClass( void )                    { return SpellClass; };
	int            GetManaCost( void )                      { return ManaCost; }
	int            GetSpellNumber( void )                   { return SpellNumber; };
	int            GetAttackCount( void )                   { return AttackCount; };
	int            GetMinDamage( void )                     { return MinDamage; };
	int            GetMaxDamage( void )                     { return MaxDamage; };
	string		   GetSpellString( void )                   { return SpellString; };

	// Set values
	void      SetSpellNumber( int number )                  { SpellNumber = number; };
	void      SetSpellName( string Name )					{ SpellName = Name; };
	void      SetSpellDescription(string Desc )				{ SpellDescription = Desc; };
	void      SetSpellCommand( string Command )				{ SpellCommand = Command; };
	void      SetSpellType( int sType )                     { SpellType = sType; };
	void      SetSpellClass( int sClass )                   { SpellClass = sClass; };
	void      SetManaCost( int mana )                       { ManaCost = mana; };
	void      SetAttackCount( int count )                   { AttackCount = count; };
	void      SetMinDamage( int damage )                    { MinDamage = damage; };
	void      SetMaxDamage( int damage )                    { MaxDamage = damage; };
	void      SetSpellString( string sString )				{ SpellString = sString; };
};

/*===========================================================================
BaseTimeSpell class

This is a base class for all non-melee based spells.  All non-melee spells
must derive from this base class.
(heal, damage over time, heal over time, curse, smite, etc)

These spells are all time based meaning they can execute several times over
a period of time. This is the case for single shot instant spells also like
instant heal spells or shocking grasp that hit once and go away (do not repeat)
============================================================================*/
class BaseTimeSpell
{
	int                 SpellNumber;
	int                 SpellType;
	int                 SpellState;
	int                 EffectCount;

	string              SpellName;
	string              SpellInitText;
	string              SpellEffectText;
	string              SpellEndText;

public:
	//Get  
	int     GetSpellNumber( void )                  { return SpellNumber; };
	int     GetSpellType( void )                    { return SpellType; };
	int     GetSpellState( void )                   { return SpellState; };
	int     GetEffectCount( void )                  { return EffectCount; };
	string  GetSpellText( int which );

	// Set
	void    SetSpellState( int state )              { SpellState = state; };
	void    SetSpellCount( int count )              { EffectCount = count; };


	//virtual
	//void    execSpell( void )=0;
};



bool LoadMeleeSpells( void );
void CastMeleeSpell( Connection *Player, char *Attacked, MeleeSpell *Spell );



typedef std::map<std::string, MeleeSpell *> MeleeSpellList;
typedef std::map<std::string, BaseTimeSpell *> TimeSpellList;
