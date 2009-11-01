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

#define MAX_ITEM_STRING		20
#define MAX_DESC_LENGTH		200
#define ITEM_DATABASE		"minions.db"


class Item
{
	unsigned int		ItemNumber;
	unsigned int		Value;
	unsigned int		Weight;
	char				Name[MAX_ITEM_STRING];
	char				Description[MAX_DESC_LENGTH];
	char				AttackType[MAX_ITEM_STRING];
	int					MaxDamage;
	int					ToHitBonus;
	int					Speed;
	int					ArmorValue;
	int					MinDamage;

public:
	Item();
	unsigned int GetItemNumber( void ) { return ItemNumber; };
	void SetItemNumber( unsigned int NewNum ) { ItemNumber = NewNum; };
	unsigned int GetItemValue( void ) { return Value; };
	void SetItemValue( unsigned int NewVal ) { Value = NewVal; };
	unsigned int GetWeight( void ) { return Weight; };
	void SetWeight( unsigned int NewWeight ) { Weight = NewWeight; };
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

};

struct ItemList
{
	ItemList *Next;
	Item	 *Value;
};

bool LoadItemDatabase( void );
Item *SearchForItem( unsigned int ItemNum );



#endif // _ITEM_H_INCLUDED
