/***************************************************************
 *           Minions Mud Server is copyright (C)2001           *
 *                    SonzoSoft Software                       *
 *                                                             *
 *         Programmed by Mark Richardson and David Brown       *
 *															   *
 *			 Mark Richardson	-	sinbaud@hotmail.com	       *
 *		       David Brown	    -	dcbrown73@yahoo.com	       *
 ***************************************************************/
#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#define	MAX_STRING_LENGTH				80
#define MAX_STATS						6
#define MAX_LEVEL_ALLOWED				150
#define MAX_DESCRIPTION_LENGTH			400
#define MAX_HOST_NAME					80
#define PLAYER_SAVE_PATH				"C:\\minions\\players\\"

class Room;  //forward declaration
class Item;  //forward declaration

/*  link list of items in this players possesion */
struct ItemsOnPlayer
{
	ItemsOnPlayer	*Next;
	Item			*Item;
};

/*  Client slash Player class definition */
class Client
{
	char			FirstName[MAX_STRING_LENGTH];
	char			LastName[MAX_STRING_LENGTH];
	char			IpAddress[MAX_STRING_LENGTH];
	char			Password[MAX_STRING_LENGTH];
	char			Description[MAX_DESCRIPTION_LENGTH];
	char			RaceStr[MAX_STRING_LENGTH];
	char			ClassStr[MAX_STRING_LENGTH];
	char			HostName[MAX_STRING_LENGTH];
	unsigned int	ArmorClass, Stealth, MaxDamage, DamageBonus;
	unsigned int	Strength, Agility, Health, Luck, Wisdom, Sex; 
	int				HitPoints, MaxHits, Mana, MaxMana, Level, Race, THAC0, Class;
	unsigned int	Exp, Kills, BeenKilled;
	Room			*CurrentRoom;
    unsigned int	CurrentRoomNumber;
	ItemsOnPlayer	*FirstItem;
	Item			*Wielded;

public:
	Client();
	void AddKill( void ) { Kills++; };
	int GetKills( void ) {return Kills; };
	void SubtractKill( void ) { Kills--; };
	char *GetFirstName( void ) { return FirstName; };
	bool SetFirstName( char *new_name );
	char *GetLastName( void ) { return LastName; };
	bool SetLastName( char *new_name );
	char *GetIpAddress( void ) { return IpAddress; };
	bool SetIpAddress( char *new_address );
	bool SetHostName( char *host );
	char *GetHostName( void ) { return HostName; };
	int GetHitPoints( void );
	int GetMaxHitPoints( void ) { return MaxHits; };
	bool SetHitPoints( int new_hits );
	int GetMana( void ) { return Mana; };
	int GetMaxMana( void ) { return MaxMana; };
	bool SetMana( int new_mana );
	int GetLevel( void ) { return Level; };
	bool SetLevel( int new_level );
	bool SetDesc( char *new_desc );
	char *GetDesc( void ) { return Description; };
	bool LoadPlayer( char *name );
	bool SavePlayer( void );
	bool SetPassword( char *new_password );
	char* GetPassword( void ) { return Password; };
	int GetRace( void ) { return Race; };
	bool SetRace( int NewRace ) { Race = NewRace; return true; };
	bool SetRaceStr( char *NewStr );
	char *GetRaceStr( void ) { return RaceStr; };
	int GetSex( void ) { return Sex; };
	void SetSex( int NewSex ) { Sex = NewSex; }; 
	Room *GetRoom( void ) { return CurrentRoom; };
	bool SetRoom( Room *NewRoom );
	unsigned int GetCurrentRoomNumber( void ) { return CurrentRoomNumber; };
	void SetCurrentRoomNumber( unsigned int NewNum ) { CurrentRoomNumber = NewNum; };
	bool AddItemToPlayer( Item *NewItem );
	bool RemoveItemFromPlayer( Item *ItemToDelete );
	ItemsOnPlayer *GetFirstItem( void ) { return FirstItem; };
	Item *SearchPlayerForItem( char *Name );
	bool WieldItem( Item *ToWield );
	Item *GetWieldedItem( void ) { return Wielded; };
	bool Client::UpdateHitPoints( int value, bool add_subtract );
	int GetTHAC0( void ) { return THAC0; };
	int GetArmorClass( void ) { return ArmorClass; };
	void DropAllItems( void );
	unsigned int GetStrength( void ) { return Strength; };
	unsigned int GetExp( void ) { return Exp; };
	void AlterExp( int ToAdd );
	unsigned int GetHealth( void ) { return Health; };
	unsigned int GetLuck( void ) { return Luck; };
	unsigned int GetAgility( void ) { return Agility; };
	unsigned int GetWisdom( void ) { return Wisdom; };
	bool SetClassStr( char *NewStr );
	char *GetClassStr( void ) { return ClassStr; };
	bool SetClass( int ClassNum );
	int GetBeenKilled( void ) { return BeenKilled; };
	void AddBeenKilled( void ) { BeenKilled++; };
	~Client();
};

#endif //PLAYER_H_INCLUDED