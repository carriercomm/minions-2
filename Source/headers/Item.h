/***************************************************************
 *           Minions Mud Server is copyright (C)2001           *
 *                    SonzoSoft Software                       *
 *                                                             *
 *         Programmed by Mark Richardson and David Brown       *
 *															   *
 *			 Mark Richardson	-	sinbaud@hotmail.com	       *
 *		       David Brown	    -	dcbrown73@yahoo.com	       *
 ***************************************************************/
#ifndef _ITEM_H_INCLUDED
#define _ITEM_H_INCLUDED

const int MAX_ITEM_STRING			= 35;
const int MAX_DESC_LENGTH		    = 200;
const char ITEM_DATABASE[]	    	= "minions.db";

// Define item types
const int ITEM_WEAPON				= 1;
const int ITEM_WEAR				    = 2;
const int ITEM_OTHER		        = 3;

// Define where item worn
const int ITEM_WIELDED				= 0;
const int ITEM_WEAR_HEAD			= 1;
const int ITEM_WEAR_ARMS			= 2;
const int ITEM_WEAR_TORSO           = 3;
const int ITEM_WEAR_LEGS			= 4;
const int ITEM_WEAR_FINGER			= 5;
const int ITEM_WEAR_FEET			= 6;
const int ITEM_WEAR_NECK			= 7;
const int ITEM_WEAR_HANDS           = 8;

class Item
{
	unsigned int		ItemNumber;
	unsigned int		Value;
	int                 ItemType;
	int					Weight;
	char				Name[MAX_ITEM_STRING];
	char				Description[MAX_DESC_LENGTH];
	char				AttackType[MAX_ITEM_STRING];
	int					MaxDamage;
	int					ToHitBonus;
	int					Speed;
	int					ArmorValue;
	int					MinDamage;
	int                 WearLocation;

public:
	Item();
	unsigned int GetItemNumber( void ) { return ItemNumber; };
	void SetItemNumber( unsigned int NewNum ) { ItemNumber = NewNum; };
	unsigned int GetItemValue( void ) { return Value; };
	void SetItemValue( unsigned int NewVal ) { Value = NewVal; };
	void SetItemType( int item_type ) { ItemType = item_type; };
	int GetItemType( void ) { return ItemType; };
	int GetWeight( void ) { return Weight; };
	void SetWeight( int NewWeight ) { Weight = NewWeight; };
	bool SetItemName( char *NewName );
	char *GetItemName( void ) { return Name; };
	bool SetItemDesc( char *NewDesc );
	char *GetItemDesc( void ) { return Description; };
	int GetMaxDamage( void ) { return MaxDamage; };
	int GetMinDamage( void ) { return MinDamage; };
	void SetMinDamage( int NewDam ) { MinDamage = NewDam; };
	void SetMaxDamage( int NewDam ) { MaxDamage = NewDam; };
	int GetToHitBonus( void ) { return ToHitBonus; };
	void SetToHitBonus( int NewBonus ) { ToHitBonus = NewBonus; };
	int GetSpeed( void ) { return Speed; };
	void SetSpeed( int NewSpeed ) { Speed = NewSpeed; };
	int GetArmorValue( void ) { return ArmorValue; };
	void SetArmorValue( int NewAV ) { ArmorValue = NewAV; };
	bool SetAttackType( char *String );
	char *GetAttackType( void ) { return AttackType; };
	void SetWearLocation( int location ) { WearLocation = location; };
	int GetWearLocation( void ) { return WearLocation; };

};

struct ItemList
{
	ItemList *Next;
	Item	 *Value;
};

bool LoadItemDatabase( void );
Item *SearchForItem( unsigned int ItemNum );



#endif // _ITEM_H_INCLUDED
