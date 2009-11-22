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
#include <sstream>
#include <string>
#include <iomanip>
#include <string.h>
#include <fstream>
#include <ctype.h>
#include "player.h"
#include "ansicolor.h"
#include "room.h"
#include "tcpcomm.h"
#include "item.h"
#include "races.h"
#include "ansicolor.h"

using namespace std;

Client::Client()
{
	FirstName[0] = LastName[0] = IpAddress[0] = Password[0] = HostName[0] = RaceStr[0] = ClassStr[0] = NULL;
	Description[0] = NULL;
	strcpy( FirstName, "-Login" );
	strcpy( LastName, "In Progress -" );
	strcpy( Description, "Constructor Default Description" );
	Stealth = MaxDamage = DamageBonus = 25;
	Strength = Agility = Health = Luck = Wisdom = 32; 
	HitPoints = MaxHits = Mana = MaxMana = 120;
	Level = Sex             = 1;
	Exp = Kills             = 0;
	Weight                  = 0;
	CurrentRoom             = NULL;
	BeenKilled              = 0;
	FirstItem               = NULL;
	Wielded                 = NULL;
	Head                    = NULL;
	Neck                    = NULL;
	Arms                    = NULL;
	Torso                   = NULL;
	Legs                    = NULL;
	Feet                    = NULL;
	Finger                  = NULL;
	Hands                   = NULL;
	CurrentRoomNumber       = 1;
	ArmorClass              = 5;
	THAC0                   = 14;
	Class = Race            = 0;
	AttackEvent             = NULL;
	MyConnection            = NULL;
	Wielded = Head = Neck = Arms = Torso = Legs = Feet = Finger = Hands = NULL; //set these to NULL in constructor
	
}

bool Client::SetRoom( Room *NewRoom )
{
	if( NewRoom == NULL )
		return false;

	CurrentRoom = NewRoom;

	return true;
}

bool Client::SetFirstName( char *new_name )
{
	char *temp = new_name;

	while( *temp )
	{
		if( *temp == ' ' )
		{
			*temp = '\0';
			break;
		}

		temp++;
	}

	if( strlen( new_name ) >= MAX_NAME_LENGTH )
		return false;

	if( strlen( new_name ) <= 2 )
		return false;

	FirstName[0] = '\0';
	strcpy( FirstName, new_name );
	FirstName[0] = toupper( FirstName[0] );
	return true;
}


bool Client::SetLastName( char *new_name )
{
	char *temp = new_name;

	while( *temp )
	{
		if( *temp == ' ' )
		{
			*temp = '\0';
			break;
		}

		*temp++;
	}

	if( strlen( new_name ) >= MAX_NAME_LENGTH )
		return false;

	if( strlen( new_name ) <= 2 )
		return false;
	
	LastName[0] = '\0';
	strcpy( LastName, new_name );
	LastName[0] = toupper( LastName[0] );
	return true;
}

bool Client::SetRaceStr( char *NewStr )
{
	if( strlen( NewStr ) >= MAX_STRING_LENGTH )
		return false;

	RaceStr[0] = '\0';
	strcpy( RaceStr, NewStr );

	return true;
}

bool Client::SetClassStr( char *NewStr )
{
	if( strlen( NewStr ) >= MAX_STRING_LENGTH )
		return false;

	ClassStr[0] = '\0';
	strcpy( ClassStr, NewStr );
	return true;
}

bool Client::SetClass( int ClassNum )
{
	if( ClassNum > 0 )
	{
		Class = ClassNum;
		return true;
	}

	return false;
}

bool Client::SetIpAddress( char *new_address )
{
	if( strlen( new_address ) >= MAX_STRING_LENGTH )
		return false;

	memset( IpAddress, '\0', MAX_STRING_LENGTH );
	strcpy( IpAddress, new_address );
	return true;
}

bool Client::SetHostName( char *host )
{
	if( host == NULL )
	{
		strcpy( HostName, "un-resolvable hostname" );
		return true;
	}

	char *temp = host;

	while( *temp )
	{
		if( *temp == ' ' )
		{
			*temp = '\0';
			break;
		}

		temp++;
	}

	if( strlen( host ) >= MAX_STRING_LENGTH )
		return false;

	HostName[0] = '\0';
	strcpy( HostName, host );
	return true;
}

int Client::GetHitPoints( void )
{
	return HitPoints;
}

bool Client:: SetHitPoints( int new_hits )
{
	if( new_hits >= 5000 )
		return false;
	
	HitPoints = new_hits;
	return true;
}


bool Client::SetMana( int new_mana )
{
	if( new_mana >= 5000 )
		return false;

	Mana = new_mana;
	return true;
}


bool Client::SetLevel( int new_level )
{
	if( new_level >= MAX_LEVEL_ALLOWED )
		return false;

	Level = new_level;
	return true;
}

bool Client::SavePlayer(void) {
	ofstream	out;
	char		filename[80] = PLAYER_SAVE_PATH;

	strcat( filename, FirstName );
	strcat( filename, ".txt" );
	out.open( filename );

	if( !out )
	{
		cout<<"Unable to open file: "<<filename;
		return false;
	}

	out<<FirstName<<' '<<LastName<<' '<<Password<<' '<<Sex<<' ';
	out<<ArmorClass<<' '<<Stealth<<' '<<MaxDamage<<' '<<DamageBonus<<' ';
	out<<Strength<<' '<<Agility<<' '<<Health<<' '<<Luck<<' '<<Wisdom<<' '; 
	out<<HitPoints<<' '<<MaxHits<<' '<<Mana<<' '<<MaxMana;
	out<<' '<<CurrentRoomNumber<<' '<<Exp<<' '<<Race<<' '<<Class;

	out.close();
	 
	return true;
}

bool Client::LoadPlayer(char *name) {
	ifstream	in;
	char		filename[80] = PLAYER_SAVE_PATH;
	
	strcat( filename, name );
	strcat( filename, ".txt" );
	in.open( filename,ios::_Nocreate );

	if( !in )
		return false;
	
	in>>FirstName>>LastName>>Password>>Sex;
	in>>ArmorClass>>Stealth>>MaxDamage>>DamageBonus;
	in>>Strength>>Agility>>Health>>Luck>>Wisdom; 
	in>>HitPoints>>MaxHits>>Mana>>MaxMana>>CurrentRoomNumber;
	in>>Exp>>Race>>Class;

	SetRaceStr( LookupRaceStr( Race ) );
	SetClassStr( LookupClassStr( Class ) );
	in.close();
	return true;
	
}

bool Client::SetPassword( char *new_password )
{
	char *temp = new_password;

	while( *temp )
	{
		if( *temp == ' ' )
		{
			*temp = '\0';
			break;
		}

		temp++;
	}

	if( strlen( new_password ) >= MAX_STRING_LENGTH )
		return false;

	if( strlen( new_password ) <= 2 )
		return false;

	Password[0] = '\0';
	strcpy( Password, new_password );
	return true;
}

bool Client::UpdateHitPoints( int value, bool add_subtract )
{
	
	if( add_subtract )
	{
		if( (HitPoints + value) > MaxHits ) 
		{
			HitPoints = MaxHits;
			return true;
		} 
		else 
		{
			HitPoints = HitPoints + value;
			return true;
		}
	} 
	else 
	{
		if( (HitPoints - value) < 1 )
		{
            HitPoints = MaxHits;
	     	return false;
		}
		else
		{
			HitPoints = HitPoints - value;
			return true;
		}
	}
}

void Client::DropAllItems( void )
{
	ItemsOnPlayer	*ListPtr = '\0';
	ItemsOnPlayer	*ListPtr_Next = '\0';
	bool WeightSet;

	// Remove anything and apply it back to the non-worn inventory
	WearItem( NULL, ITEM_WEAR_HEAD );
	WearItem( NULL, ITEM_WEAR_ARMS );
	WearItem( NULL, ITEM_WEAR_TORSO );
	WearItem( NULL, ITEM_WEAR_LEGS );
	WearItem( NULL, ITEM_WEAR_FINGER );
	WearItem( NULL, ITEM_WEAR_FEET );
	WearItem( NULL, ITEM_WEAR_NECK );
	WearItem( NULL, ITEM_WEAR_HANDS );
	WearItem( NULL, ITEM_WIELDED );

	// Begin the dropping all items in inventory
	ListPtr = FirstItem;

	if( FirstItem )
	{
		for( ListPtr = FirstItem; ListPtr; ListPtr = ListPtr_Next )
		{
			ListPtr_Next = ListPtr->Next;	

			if( CurrentRoom )
				CurrentRoom->AddItemToRoom( ListPtr->Item );
			delete ListPtr;
		}
	}

	FirstItem = NULL;
	// Set player weight to exactly zero.
	WeightSet = SetPlayerWeight( 0, EXACTLY );
	return;
}


bool Client::AddItemToPlayer( Item *NewItem )
{
	ItemsOnPlayer	*TempItemsOnPlayer;
	ItemsOnPlayer	*PlaceHolder;
	Connection      *Player;
	
	TempItemsOnPlayer = new ItemsOnPlayer;

	if( !TempItemsOnPlayer )
	{
		ServerLog( "Error allocating new TempItemsOnPlayer. Out of memory" );
		return false;
	}

	if( FirstItem )
	{
		PlaceHolder = FirstItem;
		FirstItem = TempItemsOnPlayer;
		FirstItem->Next = PlaceHolder;
		FirstItem->Item = NewItem;
		return true;
	}
	
	FirstItem = TempItemsOnPlayer;
	FirstItem->Next = '\0';
	FirstItem->Item = NewItem;
	
	return true;

}

/*=======================================================
RemoveItemFromPlayer -> Client

Removes item from a player
=======================================================*/

bool Client::RemoveItemFromPlayer( Item *ItemToDelete )
{
	ItemsOnPlayer	*Temp, *ToDelete;
	bool tmp;

	if( FirstItem->Item == ItemToDelete ) 
	{
		ToDelete = FirstItem;
		FirstItem = FirstItem->Next;
	}

	else
	{
		for( Temp = FirstItem; Temp; Temp = Temp->Next )
		{
			if( Temp->Next->Item == ItemToDelete )
				break;
		}

		ToDelete = Temp->Next;
		Temp->Next = Temp->Next->Next;
	}
	
	delete ToDelete;
	
	return true;
}
/*================================================================
Client::SearchPlayerForItem( char *Name )

Search players linked list for an item to see if the player
is in posession of it.
================================================================*/

Item *Client::SearchPlayerForItem( char *Name )
{
	ItemsOnPlayer	*ItemList = '\0';
	int				Len = strlen( Name );	

	// Search player inventory first.
	for( ItemList = FirstItem; ItemList; ItemList = ItemList->Next )
	{
		if( !strnicmp( ItemList->Item->GetItemName(), Name, Len ) )
		{
			return ItemList->Item;
		}
	}
	// If not found in inventory, search worn/wielded items 
	// A loop though a list of some sort would be better, FIX THIS later!
	if (Wielded != NULL)
	{
		if ( !strnicmp( Wielded->GetItemName(), Name, Len ) )
			return Wielded;
	}
	else if (Head != NULL)
	{
	 	if ( !strnicmp( Head->GetItemName(), Name, Len ) )
			return Head;
	}
	else if (Neck != NULL)
	{
	 	if ( !strnicmp( Neck->GetItemName(), Name, Len ) )
			return Neck;
	}
	else if (Arms != NULL)
	{
	 	if ( !strnicmp( Arms->GetItemName(), Name, Len ) )
			return Arms;
	}
	else if (Torso != NULL)
	{
	 	if ( !strnicmp( Torso->GetItemName(), Name, Len ) )
			return Torso;
	}
	else if (Legs != NULL)
	{
	 	if ( !strnicmp( Legs->GetItemName(), Name, Len ) )
			return Legs;
	}
	else if (Feet != NULL)
	{
	 	if ( !strnicmp( Feet->GetItemName(), Name, Len ) )
			return Feet;
	}
	else if ( Finger != NULL)
	{
	 	if ( !strnicmp( Finger->GetItemName(), Name, Len ) )
			return Finger;
	}
	else if ( Hands != NULL)
	{
	 	if ( !strnicmp( Hands->GetItemName(), Name, Len ) )
			return Hands;
	}

	return NULL;
}

/*================================================================
Client::SearchPlayerInventoryForItem( char *Name )

Search players linked list for an item to see if the player
is in posession of it.
================================================================*/

Item *Client::SearchPlayerInventoryForItem( char *Name )
{
	ItemsOnPlayer	*ItemList = '\0';
	int				Len = strlen( Name );	

	for( ItemList = FirstItem; ItemList; ItemList = ItemList->Next )
	{
		if( !strnicmp( ItemList->Item->GetItemName(), Name, Len ) )
		{
			return ItemList->Item;
		}
	}

	return NULL;
}


/* ====================================================
_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_
Client::WieldItem()

This function needs to be removed.  Need to make sure
it isn't used anywhere first
_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_
=====================================================*/
bool Client::WieldItem( Item *ToWield )
{
	Wielded = ToWield;
	return true;
}

/*=====================================================
Client::~Client() - Player class distructor
=====================================================*/

Client::~Client()
{
	ItemsOnPlayer	*Temp, *Temp_Next;
	int	Count = 0;

	if( FirstItem )
	{
		for( Temp = FirstItem; Temp; Temp = Temp_Next )
		{
			Temp_Next = Temp->Next;
			delete Temp;
			Count++;
		}

		cout<<"Client destructor called"<<endl;
		cout<<"Deleted "<<Count<<" items from player: "<<FirstName<<endl;;
	}
		
	else
	{
		cout<<"Client Destructor called."<<endl;
		cout<<"There were no items on the player: "<<FirstName<<endl;
	}
}

	
/*=====================================================================
 Client -> SetRestingStatus()

 Sets the resting status of the player.
 =====================================================================*/

bool Client::SetRestingStatus( int rest_status )
{
	Resting = rest_status;
	return true;
}

/*=====================================================================
Client -> SetPlayerWeight(int value, int add_subtract)

Accepts two ints and adds or substracts from the current value
=====================================================================*/
bool Client::SetPlayerWeight( int value, int add_subtract )
{
	switch (add_subtract)
	{
	case ADD:  // Don't added if over player's maximum strength weight
		if ( (Weight + value) > (Strength * MAX_STR_MULTIPLIER) )
			return false;
		Weight += value;
		return true;
		break;
	case SUBTRACT:
		Weight -= value;
		return true;
		break;
	case EXACTLY: // Don't set weight if over player's maximum strength weight
		if ( (Weight + value) > (Strength * MAX_STR_MULTIPLIER) )
			return false;
		Weight = value;
		return true;
	default:
		ServerLog("SetPlayerWeight() encountered default case switch value: %d", add_subtract);
		return false;
		break;
	}
}

/************************************************************************
	Client -> GetDescription(void)
	Build up the description string, then return a pointer to it.
*************************************************************************/
char* Client::GetDescription( void ) 
{
	//char				TempDesc[MAX_DESCRIPTION_LENGTH];
	char				HealthStatusDesc[20];
	char				WieldedDesc[MAX_STRING_LENGTH];
	char				StrDesc[40],AgilityDesc[20], HealthDesc[20], LuckDesc[20], WisdomDesc[20];
	ostringstream		BuildString;
	string				FormattedString;

	/* get the health status string */
	if( HitPoints <= ( MaxHits * .15 ) )
		strcpy( HealthStatusDesc, "critically");
	if( HitPoints <= ( MaxHits * .25 ) )
		strcpy( HealthStatusDesc, "heavily" );
	else if( HitPoints <= ( MaxHits * .50 ) )
		strcpy( HealthStatusDesc, "moderately" );
	else if( HitPoints <= ( MaxHits * .75 ) )
		strcpy( HealthStatusDesc, "slighlty" );
	else if( HitPoints <= ( MaxHits * .95 ) )
		strcpy( HealthStatusDesc, "mildly" );
	else
		strcpy( HealthStatusDesc, "un" );

	/* get the strength description string */
	if( Strength <= ( MaxHits * .15 ) )
		strcpy( StrDesc, "a sick looking");
	if( Strength <= ( MaxHits * .25 ) )
		strcpy( StrDesc, "a weak ass" );
	else if( Strength <= ( MaxHits * .50 ) )
		strcpy( StrDesc, "an average built" );
	else if( Strength <= ( MaxHits * .75 ) )
		strcpy( StrDesc, "a powerfully built" );
	else if( Strength <= ( MAX_STR_MULTIPLIER * .95 ) )
		strcpy( StrDesc, "very powerfully built" );
	else
		strcpy( StrDesc, "extremely powerfully built" );

	/*  added STL strings here for formatting  */
	BuildString<<ANSI_CLR_SOL<<ANSI_BR_GREEN<<'['<<FirstName<<' '<<LastName<<"] - "<<ANSI_BR_WHITE
		<<RaceStr<<' '<<ClassStr<<"\n\r";

	/* do general description here  */
	BuildString<<ANSI_WHITE<<FirstName<<" is "<<StrDesc<<' '<<RaceStr<<' '<<ClassStr<<"\n\r"<<ANSI_BR_YELLOW;

	/* display if they are wielding or wearing things */
	if( Head )
		BuildString<<Head->GetItemName()<<" (Head)\n\r";
	if( Neck )
		BuildString<<Neck->GetItemName()<<" (Neck)\n\r";
	if( Arms )
		BuildString<<Arms->GetItemName()<<" (Arm)\n\r";
	if( Torso )
		BuildString<<Torso->GetItemName()<<" (Torso)\n\r";
	if( Legs )
		BuildString<<Legs->GetItemName()<<" (Legs)\n\r";
	if( Feet )
		BuildString<<Feet->GetItemName()<<" (Feet)\n\r";
	if( Finger )
		BuildString<<Finger->GetItemName()<<" (Finger)\n\r";
	if( Hands )
		BuildString<<Hands->GetItemName()<<" (Hand)\n\r";
	if( Wielded )
		BuildString<<Wielded->GetItemName()<<" (Weapon Hand)\n\r"<<ANSI_BR_GREEN;

	BuildString<<FirstName<<" is "<<HealthStatusDesc<<"-wounded.\n\r"<<ANSI_BR_WHITE;

	/* convert to std::string type */
	FormattedString = BuildString.str();

	if( strlen( FormattedString.c_str() ) >= MAX_DESCRIPTION_LENGTH )
		strcpy( Description, "Description Too Long in Client::GetDescription()" );
	else
		strcpy( Description, FormattedString.c_str() );

	return Description;
}
	
/*============================================================
Client::IsWearing()

Check to see if the player is wearing something at noted location
============================================================*/

Item* Client::IsWearing( int wearing )
{
	switch ( wearing )
	{
	case ITEM_WIELDED:
		if (Wielded != NULL)
			return Wielded;
	case ITEM_WEAR_HEAD:
		if (Head != NULL)
			return Head;
		break;
	case ITEM_WEAR_NECK:
		if (Neck != NULL)
			return Neck;
		break;
	case ITEM_WEAR_TORSO:
		if (Torso != NULL)
			return Torso;
		break;
	case ITEM_WEAR_ARMS:
		if (Arms != NULL)
			return Arms;
		break;
	case ITEM_WEAR_LEGS:
		if (Legs != NULL)
			return Legs;
		break;
	case ITEM_WEAR_FINGER:
		if (Finger != NULL)
			return Finger;
		break;
	case ITEM_WEAR_FEET:
		if (Feet != NULL)
			return Feet;
		break;
	case ITEM_WEAR_HANDS:
		if (Hands != NULL)
			return Hands;
		break;
	default:
		cout << "Error, IsWearing() case hit default!" << endl;
		break;
	}
	return NULL;
}

			
/*============================================================
Client::WearItem()

Wear the item passed in the position passed.
============================================================*/

void Client::WearItem( Item *ItemToWear, int ItemPlacement )
{
	// Update any stats modified by item and remove / add item from local non-worn inventory
	if (ItemToWear != NULL) // Actual item, add stats and subtract and stats and INV
	{
		if ( IsWearing(ItemPlacement) )
		{
			AdjustPlayerStatsByItem( IsWearing(ItemPlacement), REMOVE );
			AddItemToPlayer( IsWearing(ItemPlacement) );
		}

		AdjustPlayerStatsByItem( ItemToWear, ADD );
		RemoveItemFromPlayer( ItemToWear );
	}
	else    // No item subtract what is being worn (IF) something is worn and add back to INV
	{
		if ( IsWearing(ItemPlacement) != NULL ) 
		{
			AdjustPlayerStatsByItem(IsWearing(ItemPlacement), REMOVE);
			AddItemToPlayer(IsWearing(ItemPlacement));
		}
	}

	// Now, wear the item.
	switch ( ItemPlacement )
	{
	case ITEM_WIELDED:
		Wielded = ItemToWear;
		break;
	case ITEM_WEAR_HEAD:
		Head = ItemToWear;
		break;
	case ITEM_WEAR_NECK:
		Neck = ItemToWear;
		break;
	case ITEM_WEAR_TORSO:
		Torso = ItemToWear;
		break;
	case ITEM_WEAR_ARMS:
		Arms = ItemToWear;
		break;
	case ITEM_WEAR_LEGS:
		Legs = ItemToWear;
		break;
	case ITEM_WEAR_FINGER:
		Finger = ItemToWear;
		break;
	case ITEM_WEAR_FEET:
		Feet = ItemToWear;
		break;
	case ITEM_WEAR_HANDS:
		Hands = ItemToWear;
		break;
	default:
		cout << "Error, WearItem() case hit default!" << endl;
		return;
		break;
	}
}


/*============================================================
Client::AdjustPlayerStatsByItem( Item *CurItem, int add_remove )

Adjust the players stats as they wield/wear or remove items
============================================================*/
void Client::AdjustPlayerStatsByItem( Item *CurItem, int add_remove )
{
	switch ( add_remove )
	{
	case ADD:
		ArmorClass     += CurItem->GetArmorValue();
		THAC0          += CurItem->GetToHitBonus();
		Strength       += CurItem->GetBonus(STRENGTH_BONUS);
		Agility        += CurItem->GetBonus(AGILITY_BONUS);
		Health         += CurItem->GetBonus(HEALTH_BONUS);
		Luck	       += CurItem->GetBonus(LUCK_BONUS);
		Wisdom         += CurItem->GetBonus(WISDOM_BONUS);
		HitPoints      += CurItem->GetBonus(HITPOINTS_BONUS);
		Mana           += CurItem->GetBonus(MANA_BONUS);
		// Add other stats as items stat modifiers get added to the Items class
		break;
	case REMOVE:
		ArmorClass     -= CurItem->GetArmorValue();
		THAC0          -= CurItem->GetToHitBonus();
		Strength       -= CurItem->GetBonus(STRENGTH_BONUS);
		Agility        -= CurItem->GetBonus(AGILITY_BONUS);
		Health         -= CurItem->GetBonus(HEALTH_BONUS);
		Luck	       -= CurItem->GetBonus(LUCK_BONUS);
		Wisdom         -= CurItem->GetBonus(WISDOM_BONUS);
		HitPoints      -= CurItem->GetBonus(HITPOINTS_BONUS);
		Mana           -= CurItem->GetBonus(MANA_BONUS);
		// add matching stats as added to above
		break;
	default:
		ServerLog( "AdjustPlayerStatsByItem(): Cannot adjust player stats by item if case is %d", add_remove );
		break;
	}
}