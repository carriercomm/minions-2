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
	CurrentRoomNumber       = 1;
	ArmorClass              = 5;
	THAC0                   = 14;
	Class = Race            = 0;
	AttackEvent             = NULL;
	MyConnection            = NULL;
	
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

	ListPtr = FirstItem;

	if( ! FirstItem )
		return;

	for( ListPtr = FirstItem; ListPtr; ListPtr = ListPtr_Next )
	{
		ListPtr_Next = ListPtr->Next;

		if( CurrentRoom )
			// If player is holding the item adjust stats accordingly when removing.
			if ( IsWearing(ListPtr->Item->GetWearLocation()) != NULL )
			{		
				AdjustPlayerStatsByItem(ListPtr->Item, REMOVE);
				WearItem(NULL, ListPtr->Item->GetWearLocation());
			}
			CurrentRoom->AddItemToRoom( ListPtr->Item );

		delete ListPtr;
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
	
	if ( !SetPlayerWeight( NewItem->GetWeight(), ADD) )
	{
		WriteToBuffer(MyConnection, "%sYou cannot carry that much weight!\n\r%s", ANSI_CYAN, ANSI_WHITE);
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
RemoveItemRomPlayer -> Client

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
	tmp = SetPlayerWeight( ItemToDelete->GetWeight(), SUBTRACT );
	AdjustPlayerStatsByItem(ItemToDelete, REMOVE);
	WearItem(NULL, ItemToDelete->GetWearLocation());
	
	return true;
}

Item *Client::SearchPlayerForItem( char *Name )
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

	return '\0';
}

bool Client::WieldItem( Item *ToWield )
{
	Wielded = ToWield;
	return true;
}


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
	char TempDesc[MAX_DESCRIPTION_LENGTH];
	char HealthStatusDesc[20];
	char WieldedDesc[MAX_STRING_LENGTH];

	/* get the health status string */
	if( HitPoints <= ( MaxHits * .95 ) )
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
		strcpy( HealthStatusDesc, "not" );

	/* get the item wielded currently  */
	if( Wielded )
		strcpy( WieldedDesc, Wielded->GetItemName() );
	else
		strcpy( WieldedDesc, "nothing." );

	/* build it into the TempDesc char array  */	
	sprintf( TempDesc, "%s%s[ %s %s ] - %s%s %s\n\r%s%s is %s wounded.\n\r%s is wielding: %s\n\r", ANSI_CLR_SOL, ANSI_BR_GREEN, FirstName,
		LastName, ANSI_BR_WHITE, RaceStr, ClassStr, ANSI_BR_YELLOW, FirstName, HealthStatusDesc, FirstName, WieldedDesc );

	if( strlen( TempDesc ) >= MAX_DESCRIPTION_LENGTH )
		strcpy( Description, "Description Too Long in Client::GetDescription()" );
	else
		strcpy( Description, TempDesc );

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
	case ITEM_WEAR_FEET:
		Feet = ItemToWear;
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
		// Add other stats as items stat modifiers get added to the Items class
		break;
	case REMOVE:
		ArmorClass     -= CurItem->GetArmorValue();
		THAC0          -= CurItem->GetToHitBonus();
		// add matching stats as added to above
		break;
	default:
		ServerLog( "AdjustPlayerStatsByItem(): Cannot adjust player stats by item if case is %d", add_remove );
		break;
	}
}