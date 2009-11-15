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

using namespace std;

Client::Client()
{
	FirstName[0] = LastName[0] = IpAddress[0] = Password[0] = HostName[0] = RaceStr[0] = ClassStr[0] = '\0';
	strcpy( FirstName, "-Login" );
	strcpy( LastName, "In Progress -" );
	Stealth = MaxDamage = DamageBonus = 25;
	Strength = Agility = Health = Luck = Wisdom = 32; 
	HitPoints = MaxHits = Mana = MaxMana = 120;
	Level = Sex = 1;
	Exp = Kills = 0;
	Weight = 0;
	CurrentRoom = NULL;
	BeenKilled = 0;
	FirstItem = NULL;
	Wielded = NULL;
	CurrentRoomNumber = 1;
	ArmorClass = 5;
	THAC0 = 14;
	Class = Race = 0;
	AttackEvent = NULL;
	MyConnection = NULL;
	
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
			CurrentRoom->AddItemToRoom( ListPtr->Item );

		delete ListPtr;
	}

	FirstItem = '\0';
	Wielded = '\0';
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
Client -> SetWeight(int value, int add_subtract)

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