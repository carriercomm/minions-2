/***************************************************************
 *           Minions Mud Server is copyright (C)2001           *
 *                    SonzoSoft Software                       *
 *                                                             *
 *         Programmed by Mark Richardson and David Brown       *
 *															   *
 *			 Mark Richardson	-	sinbaud@hotmail.com	       *
 *		       David Brown	    -	dcbrown73@yahoo.com	       *
 ***************************************************************/
#include <set>
#include <map>

using namespace std;

#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

const int MAX_STRING_LENGTH				=   80;
const int MAX_FNAME_LENGTH				=   11; //10 with one for terminating NULL
const int MAX_LNAME_LENGTH				=   15; //14 with one for terminating NULL 
const int MAX_STATS						=    6; 
const int MAX_LEVEL_ALLOWED				=  150;
const int MAX_DESCRIPTION_LENGTH		= 2048;
const int MAX_HOST_NAME					=   80;
const int MAX_STR_MULTIPLIER			=    3;
const int MAX_STAT_VALUE				=  150;
const int MAX_GESTURE_STRING            =   75;

const int MAXIMUM_STATS                 =  100;

#define PLAYER_SAVE_PATH		 "players\\"

// Sort of booleans for adding or subtracting stats in functions
const int ADD                         =   1;
const int SUBTRACT                    =   2;
const int EXACTLY                     =   3;
const int REMOVE                      =   4;

const bool FAIL                       =   false;
const bool SUCCESS                    =   true;

class Room;  //forward declaration
class Item;  //forward declaration
class minionsEvent;
struct Connection;
struct SpellEffect;

// Stats that can be set with SetPlayerStat()
const int STRENGTH                    =   1;
const int AGILITY                     =   2;
const int WISDOM                      =   3;
const int HEALTH                      =   4;
const int HP                          =   5;
const int MANA                        =   6;
const int MAX_HP                      =   7;
const int MAX_MANA                    =   8;


const int STRENGTH_DAMAGE_MODIFIER    =  10;
const int AGILITY_AC_MODIFIER         =   5;

class BaseTimeSpell;
/*  link list of items in this players possesion */
struct ItemsOnPlayer
{
	ItemsOnPlayer	*Next;
	Item			*Item;
	unsigned int	ItemCount;	//how many of this item are you in posession of
};

struct SpellOnPlayer
{
	BaseTimeSpell     *Spell;
	minionsEvent      *CurEvent;
};

typedef std::map<int, SpellOnPlayer *> SpellsEffectedByList;

/*  Client slash Player class definition */
class Client
{
	char			            FirstName[MAX_FNAME_LENGTH];
	char	             		LastName[MAX_LNAME_LENGTH];
	char						IpAddress[MAX_STRING_LENGTH];
	char						Password[MAX_STRING_LENGTH];
	char						Description[MAX_DESCRIPTION_LENGTH];
	char						RaceStr[MAX_STRING_LENGTH];
	char						ClassStr[MAX_STRING_LENGTH];
	char						HostName[MAX_STRING_LENGTH];
	char				    	CastGesture[MAX_GESTURE_STRING];
	char						CastMyGesture[MAX_GESTURE_STRING];
	unsigned int				ArmorClass, Stealth, MaxDamage, DamageBonus;
	unsigned int				Strength, Agility, Health, Luck, Wisdom, Sex; 
	int							HitPoints, MaxHits, Mana, MaxMana, Level, Race;
	int							THAC0, Class, Resting, Weight, MagicRes;
	unsigned int				Exp, Kills, BeenKilled;
	int					     	ModifiedAC;
	Room						*CurrentRoom;
    unsigned int				CurrentRoomNumber;
	ItemsOnPlayer				*FirstItem;
	minionsEvent			    *AttackEvent;
	Connection				    *MyConnection;  // So I can send stuff to buffer from member functions
	// Items worn or wielded
	Item						*Wielded;
	Item						*Head;
	Item						*Neck;
	Item						*Arms;
	Item						*Torso;
	Item						*Legs;
	Item						*Feet;
	Item						*Finger;
	Item						*Hands;
	set<int>					WearableTypes;
	set<int>					SpellTypes;
    // What spells is the player affected by
	SpellsEffectedByList		SpellEffects;



public:
	Client();
	~Client();
    // Player file IO
	bool       LoadPlayer( char *name );
	bool       SavePlayer( void );
	// Set account items
	bool       SetIpAddress( char *new_address );
	bool       SetHostName( char *host );
	void       SetConnection( Connection *Conn )                { MyConnection = Conn; };
	bool	   SetPassword( char *new_password );
	bool       SetFirstName( char *new_name );
	bool       SetLastName( char *new_name );
	bool       SetRace( int NewRace )                           { Race = NewRace; return true; };
	bool       SetRaceStr( char *NewStr );
	void       SetSex( int NewSex )                             { Sex = NewSex; };
	bool	   SetClassStr( char *NewStr );
	bool       SetClass( int ClassNum );

	// Set Stats
	bool       SetHitPoints( int new_hits );
	bool       SetMana( int new_mana );
	bool       SetLevel( int new_level );
	bool       SetDesc( char *new_desc );
	bool       SetRestingStatus( int rest_status );
	void       SetMagicResistence( int res )                    { MagicRes = res; };
	bool       SetPlayerWeight( int add_weight_value, int add_subtract );
	void       SetPlayerStat ( int value, int which_stat );


	// Get Account items
	char      *GetHostName( void )                              { return HostName; };
	char      *GetIpAddress( void )                             { return IpAddress; };
	char      *GetPassword( void )								{ return Password; };
	char      *GetFirstName( void )                             { return FirstName; };
	char      *GetLastName( void )                              { return LastName; };
	int        GetRace( void )                                  { return Race; };
	char      *GetRaceStr( void )								{ return RaceStr; };
	int        GetSex( void )									{ return Sex; };
	char	  *GetClassStr( void )                              { return ClassStr; };
	char      *GetDescription( void ); 

	// Get Stats
	int        GetHitPoints( void );
	int        GetMaxHitPoints( void )                          { return MaxHits; };
	int        GetMana( void )                                  { return Mana; };
	int        GetMaxMana( void )                               { return MaxMana; };	
	int        GetKills( void )                                 { return Kills; };	
	int        GetRestingStatus( void)                          { return Resting; };
	int        GetLevel( void )                                 { return Level; };
	int        GetTHAC0( void )                                 { return THAC0; };
	int        GetArmorClass( void )                            { return ArmorClass; };
	int        GetModifiedAC( void )							{ return ModifiedAC; };
	int        GetMagicResistence( void )                       { return MagicRes; };
	int        GetPlayerWeight( void )							{ return Weight; };
	int        GetBeenKilled( void )							{ return BeenKilled; };
	int		   GetDamageBonus ( void )                          { return DamageBonus; };
	unsigned int GetStrength( void )							{ return Strength; };
	unsigned int GetExp( void )									{ return Exp; };
	unsigned int GetHealth( void )                              { return Health; };
	unsigned int GetLuck( void )								{ return Luck; };
	unsigned int GetAgility( void )								{ return Agility; };
	unsigned int GetWisdom( void )								{ return Wisdom; };

	// Room specific
	Room           * GetRoom( void )                            { return CurrentRoom; };
	unsigned int     GetCurrentRoomNumber( void )               { return CurrentRoomNumber; };
	void             SetCurrentRoomNumber( unsigned int NewNum ){ CurrentRoomNumber = NewNum; };
	bool             SetRoom( Room *NewRoom );

	// Items specific 
	ItemsOnPlayer   *GetFirstItem( void )                       { return FirstItem; };
	void	  		 AssignWearable( set<int> Wearable )        { WearableTypes = Wearable; };
	Item            *GetWieldedItem( void )                     { return Wielded; };
	bool             AddItemToPlayer( Item *NewItem );
	bool             RemoveItemFromPlayer( Item *ItemToDelete );
	Item            *SearchPlayerForItem( char *Name );
	Item            *SearchPlayerInventoryForItem( char *Name );
	bool             WieldItem( Item *ToWield );
	void             DropAllItems( void );
	Item            *IsWearing( int wearing );
	void			 WearItem( Item *ItemToWear, int ItemPlacement);

	bool			 CanWear( int WType );

    // Utility
	void             AddKill( void )                            { Kills++; };
	void             AddBeenKilled( void )                      { BeenKilled++; };
	void             SubtractKill( void )                       { Kills--; BeenKilled++; };
	void	         UpdateModifiedStats( void );
	bool             LuckRoll( void );
	void             AlterExp( int ToAdd );
	void             AdjustPlayerStatsByItem(Item *CurItem, int add_remove );
	bool             UpdateHitPoints( int value, bool add_subtract );
	bool             TooHeavy( void );

	// Attack event stuff
	void             SetAttackEvent(minionsEvent *Event)        { AttackEvent = Event; }; 
	minionsEvent    *GetAttackEvent( void )                     { return AttackEvent; };

	// Spell related
	void             AssignSpellTypes( set<int> sTypes )        { SpellTypes = sTypes; };
	char            *GetCastGesture( void )                     { return CastGesture; };
	char            *GetCastMyGesture( void )                   { return CastMyGesture; };\
	bool             SetCastGesture( char *gesture );
	bool             SetCastMyGesture( char *gesture );
	bool             CanCast( int sType );

};

#endif //PLAYER_H_INCLUDED