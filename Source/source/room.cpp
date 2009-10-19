/***************************************************************
 *           Minions Mud Server is copyright (C)2001           *
 *                    SonzoSoft Software                       *
 *                                                             *
 *         Programmed by Mark Richardson and David Brown       *
 *															   *
 *			 Mark Richardson	-	sinbaud@hotmail.com	       *
 *		       David Brown	    -	dcbrown73@yahoo.com	       *
 ***************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <string.h>
#include "room.h"
#include "tcpcomm.h"
#include "item.h"

using namespace std;
/**********************************************************
 *  Global variables                                      *
 **********************************************************/
struct RoomList		*FirstRoomInList = '\0';
//extern MYSQL		SQLConnection;


Room::Room()
{
	RoomNumber = PlayerCount = ItemCount = 0;
	ShortDesc[0] = LongDesc[0] = ExitString[0] = '\0';
	AlsoHereString[0] = '\0';
	FirstPlayer = '\0';
	FirstItem = '\0';
	
	for( int loop = 0; loop < MAX_EXITS; loop++ )
	{
		ExitToRoomNumber[loop] = 0;
		Exit[loop] = NULL;
	}
}


bool Room::SetShortDesc( char *NewDesc )
{
	if( strlen( NewDesc ) >= MAX_SHORT_DESC )
	{
		strcpy( ShortDesc, "Default Short Description." );
		return false;
	}

	strcpy( ShortDesc, NewDesc );

	return true;
}

bool Room::SetLongDesc( char *NewDesc )
{
	if( strlen( NewDesc ) >= MAX_LONG_DESC )
	{
		strcpy( LongDesc, "Default Long Room Description." );
		return false;
	}

	strcpy( LongDesc, NewDesc );

	return true;
}

void Room::BroadcastRoom( char *format, ... )
{
	if( !FirstPlayer )
		return;

	PlayersInRoom	*TempPlayer;	
	char			TempString[MAX_BROADCAST_SIZE];
	va_list			ArgumentList;

	va_start( ArgumentList, format );
	vsprintf( TempString, format, ArgumentList );
		
	for( TempPlayer = FirstPlayer; TempPlayer; TempPlayer = TempPlayer->Next )
	{
		WriteToBuffer( TempPlayer->Player, TempString );
	}

	return;
}

void Room::SetExitString( void )
{
	char *ExitStr[] = { "north", "northeast", "east", "southeast",
		                    "south", "southwest", "west", "northwest",
							"up", "down" };
	int Count = 0;

	for( int loop = 0; loop < MAX_EXITS; loop++ )
	{
		if( Exit[loop] && Count < 1 )
		{
			strcpy( ExitString, ExitStr[loop] );
			Count++;
		}

		else if( Exit[loop] )
		{
			strcat( ExitString, ", " );
			strcat( ExitString, ExitStr[loop] );
		}
	}
}

bool Room::AddPlayerToRoom( Connection *Conn )
{
	PlayersInRoom	*TempPlayersInRoom;
	PlayersInRoom	*PlaceHolder;
	
	TempPlayersInRoom = new PlayersInRoom;

	if( !TempPlayersInRoom )
	{
		ServerLog( "Error allocating new PlayersInRoom. Out of memory in room %d\n", RoomNumber );
		return false;
	}
	
	if( FirstPlayer )
	{
		PlaceHolder = FirstPlayer;
		FirstPlayer = TempPlayersInRoom;
		FirstPlayer->Next = PlaceHolder;
		FirstPlayer->Player = Conn;
		PlayerCount++;
		return true;
	}
	
	FirstPlayer = TempPlayersInRoom;
	FirstPlayer->Next = '\0';
	FirstPlayer->Player = Conn;

	PlayerCount++;

	return true;

}

bool Room::RemovePlayerFromRoom( Connection *Conn )
{
	PlayersInRoom	*Temp, *ToDelete;

	if( FirstPlayer->Player == Conn ) 
	{
		ToDelete = FirstPlayer;
		FirstPlayer = FirstPlayer->Next;
	}

	else
	{
		for( Temp = FirstPlayer; Temp; Temp = Temp->Next )
		{
			if( Temp->Next->Player == Conn )
				break;
		}

		ToDelete = Temp->Next;
		Temp->Next = Temp->Next->Next;
	}

	delete ToDelete;
	
	PlayerCount--;

	return true;
}

/**********************************************************
 *  Link list funcs for items in room                     *
 **********************************************************/
bool Room::AddItemToRoom( Item *ItemToAdd )
{
	ItemsInRoom		*TempItemsInRoom;
	ItemsInRoom		*PlaceHolder;
	
	TempItemsInRoom = new ItemsInRoom;

	if( !TempItemsInRoom )
	{
		ServerLog( "Error allocating new ItemInRoom. Out of memory.\n" );
		return false;
	}
	
	if( FirstItem )
	{
		PlaceHolder = FirstItem;
		FirstItem = TempItemsInRoom;
		FirstItem->Next = PlaceHolder;
		FirstItem->Item = ItemToAdd;
		ItemCount++;
		return true;
	}
	
	FirstItem = TempItemsInRoom;
	FirstItem->Next = '\0';
	FirstItem->Item = ItemToAdd;

	ItemCount++;

	return true;

}

bool Room::RemoveItemFromRoom( Item *ItemToRemove )
{
	ItemsInRoom	*Temp, *ToDelete;

	if( FirstItem->Item == ItemToRemove ) 
	{
		ToDelete = FirstItem;
		FirstItem = FirstItem->Next;
	}

	else
	{
		for( Temp = FirstItem; Temp; Temp = Temp->Next )
		{
			if( Temp->Next->Item == ItemToRemove )
				break;
		}

		ToDelete = Temp->Next;
		Temp->Next = Temp->Next->Next;
	}

	delete ToDelete;
	
	ItemCount--;

	return true;
}

void Room::SetAlsoHereString( void )
{
	int Count = 0;
	PlayersInRoom	*Temp = '\0';

	AlsoHereString[0] = '\0';
	
	if( !FirstPlayer )
		return;
		
	for( Temp = FirstPlayer; Temp; Temp = Temp->Next )
	{
		if( Count < 1 )
		{
			strcpy( AlsoHereString, Temp->Player->Player.GetFirstName() );
			Count++;
		}

		else
		{
			strcat( AlsoHereString, ", " );
			strcat( AlsoHereString, Temp->Player->Player.GetFirstName() );
		}
	}

}

char *Room::GetAlsoHereString( Connection *Conn )
{
	int Count = 0;
	PlayersInRoom	*Temp = '\0';

	AlsoHereString[0] = '\0';
	
	if( !FirstPlayer )
		return '\0';
		
	for( Temp = FirstPlayer; Temp; Temp = Temp->Next )
	{
		if( Temp->Player == Conn )
			continue;

		if( Count < 1 )
		{
			strcpy( AlsoHereString, Temp->Player->Player.GetFirstName() );
			Count++;
		}

		else
		{
			strcat( AlsoHereString, ", " );
			strcat( AlsoHereString, Temp->Player->Player.GetFirstName() );
		}
	}

	return AlsoHereString;
}

Connection *Room::SearchRoomForPlayer( char *Name )
{
	PlayersInRoom	*PlayerList = '\0';
	int				Len = strlen( Name );

	for( PlayerList = FirstPlayer; PlayerList; PlayerList = PlayerList->Next )
	{
		if( !strnicmp( PlayerList->Player->Player.GetFirstName(), Name, Len ) )
		{
			return PlayerList->Player;
		}
	}

	return '\0';
}

void Room::SelectiveBroadcast( Connection *Actor, Connection *Victim, char *Format, ... )
{
	if( !Actor || !FirstPlayer )
		return;
	
	PlayersInRoom	*TempPlayer;	
	char			TempString[MAX_BROADCAST_SIZE];
	va_list			ArgumentList;

	va_start( ArgumentList, Format );
	vsprintf( TempString, Format, ArgumentList );
		
	for( TempPlayer = FirstPlayer; TempPlayer; TempPlayer = TempPlayer->Next )
	{
		if( TempPlayer->Player == Actor || TempPlayer->Player == Victim )
			continue;

		WriteToBuffer( TempPlayer->Player, TempString );
	}

	return;
}

Item *Room::SearchRoomForItem( char *Name )
{
	ItemsInRoom *ItemList = '\0';
	int	Len = strlen( Name );

	for( ItemList = FirstItem; ItemList; ItemList = ItemList->Next )
	{
		if( !strnicmp( ItemList->Item->GetItemName(), Name, Len ) )
		{
			return ItemList->Item;
		}
	}

	return '\0';
}

Room::~Room()
{
	PlayersInRoom	*Temp, *Temp_Next;
	ItemsInRoom		*TempItem, *TempItem_Next;
	int	Count = 0;

	if( FirstPlayer )
	{
		for( Temp = FirstPlayer; Temp; Temp = Temp_Next )
		{
			Temp_Next = Temp->Next;
			delete Temp;
			Count++;
		}

		cout<<"Room destructor called"<<endl;
		cout<<"Deleted "<<Count<<" players from room: "<<RoomNumber;
		cout<<" - PlayerCount was supposedly: "<<PlayerCount<<endl;
	}
		
	else
	{
		cout<<"Room Destructor called."<<endl;
		cout<<"There were no players in room: "<<RoomNumber;
		cout<<" - PlayerCount was supposedly: "<<PlayerCount<<endl;
	}

	Count = 0;

	if( FirstItem )
	{
		for( TempItem = FirstItem; TempItem; TempItem = TempItem_Next )
		{
			TempItem_Next = TempItem->Next;
			delete TempItem;
			Count++;
		}

		cout<<"Deleted "<<Count<<" items from room: "<<RoomNumber;
		cout<<" - ItemCount was supposedly: "<<ItemCount<<endl;
	}
		
	else
		cout<<"There were no items in room: "<<RoomNumber<<endl;
	
}


/**********************************************************
 *  Functions that are not part of the Room class         *
 **********************************************************/
int LoadRoomDatabase( void )
{
	//MYSQL_RES	*Result;
	//MYSQL_ROW	RowOfData;
	RoomList	*RoomPtr = '\0';
	int			NumFields = 0, Dir = 0;
	FirstRoomInList = new RoomList;
	RoomPtr = FirstRoomInList;

	RoomPtr->Next = '\0';
	
	RoomPtr->Value = new Room;
	RoomPtr->Value->SetRoomNumber( 1 );
	RoomPtr->Value->SetShortDesc( "Town Square" );
	RoomPtr->Value->SetLongDesc( "This is main street square in minionsville." );
	RoomPtr->Value->SetExitToRoomNumber( 0, 2 );
	RoomPtr->Value->SetExitToRoomNumber( 6, 3 );
	
	RoomPtr->Next = new RoomList;
	RoomPtr = RoomPtr->Next;
	RoomPtr->Value = new Room;
	RoomPtr->Value->SetRoomNumber( 2 );
	RoomPtr->Value->SetShortDesc( "North of town square" );
	RoomPtr->Value->SetLongDesc( "This is north of the main street square in minionsville." );
	RoomPtr->Value->SetExitToRoomNumber( 4, 1 );

	RoomPtr->Next = new RoomList;
	RoomPtr = RoomPtr->Next;
	RoomPtr->Value = new Room;
	RoomPtr->Value->SetRoomNumber( 3 );
	RoomPtr->Value->SetShortDesc( "Minions Bank" );
	RoomPtr->Value->SetLongDesc( "Lobby of the bank of minions." );
	RoomPtr->Value->SetExitToRoomNumber( 2, 1 );
	RoomPtr->Value->SetExitToRoomNumber( 9, 4 );

	RoomPtr->Next = new RoomList;
	RoomPtr = RoomPtr->Next;
	RoomPtr->Value = new Room;
	RoomPtr->Value->SetRoomNumber( 4 );
	RoomPtr->Value->SetShortDesc( "Bank Vault" );
	RoomPtr->Value->SetLongDesc( "What the hell goes on down here?" );
	RoomPtr->Value->SetExitToRoomNumber( 8, 4 );

	RoomPtr->Next = '\0';
	return 1;

	//if ( mysql_query( &SQLConnection, "SELECT * FROM rooms" ) )
		//exiterr(3);

	//if ( !( Result = mysql_use_result( &SQLConnection ) ) )
		//exiterr(4);


	//FirstRoomInList = new RoomList;
	//RoomPtr = FirstRoomInList;

	//RowOfData = mysql_fetch_row( Result );

	//for( ;; )
	//{
		//if( !RowOfData )
			//break;

		//RoomPtr->Next = '\0';
		//RoomPtr->Value = new Room;

		//RoomPtr->Value->SetRoomNumber( atoi( RowOfData[0] ) );
		//RoomPtr->Value->SetShortDesc( RowOfData[1] );
		//RoomPtr->Value->SetLongDesc( RowOfData[2] );

		//Dir = 0;
		//for( int loop = 3; loop < 13; loop++ )
		//{
			
			//if( !RowOfData[loop] )
			//{
				//Dir++;
				//continue;
			//}

			//RoomPtr->Value->SetExitToRoomNumber( Dir, atoi( RowOfData[loop] ) );
			//Dir++;
		//}
		
		//if( !(RowOfData = mysql_fetch_row( Result ) ) )
		//{
			//RoomPtr->Next = '\0';
			//break;
		//}

		//RoomPtr->Next = new RoomList;
		//RoomPtr = RoomPtr->Next;
	//}

	//return 1;
}


void FixExits( void )
{
	RoomList *ListPtr = '\0';
	Room	 *RomPtr = '\0';

	for( ListPtr = FirstRoomInList; ListPtr && ListPtr->Value; ListPtr = ListPtr->Next )
	{
		for( int loop = 0; loop < MAX_EXITS; loop++ )
		{
			if( ListPtr->Value->GetExitToRoomNumber( loop ) != 0 )
			{
				RomPtr = SearchForRoom( ListPtr->Value->GetExitToRoomNumber( loop ) );
				
				if( !RomPtr )
					ListPtr->Value->SetExit( '\0', loop );
	
				else
					ListPtr->Value->SetExit( RomPtr, loop );
			}

			else
				ListPtr->Value->SetExit( '\0', loop );
		}
		ListPtr->Value->SetExitString();
	}
}

Room *SearchForRoom( unsigned int RoomNum )
{
	RoomList *ListPtr = '\0';

	for( ListPtr = FirstRoomInList; ListPtr && ListPtr->Value; ListPtr = ListPtr->Next )
	{
		if( ListPtr->Value->GetRoomNumber() == RoomNum )
			return ListPtr->Value;

	}

	return '\0';
}

void exiterr(int exitcode)
{
	//ServerLog( "%s\n", mysql_error( &SQLConnection ) );
	exit( exitcode );
}