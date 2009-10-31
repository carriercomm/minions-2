/***************************************************************
 *           Minions Mud Server is copyright (C)2001           *
 *                    SonzoSoft Software                       *
 *                                                             *
 *         Programmed by Mark Richardson and David Brown       *
 *															   *
 *			 Mark Richardson	-	sinbaud@hotmail.com	       *
 *		       David Brown	    -	dcbrown73@yahoo.com	       *
 ***************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <string.h>
#include "room.h"
#include "tcpcomm.h"
#include "item.h"
#include "sqlite3.h"

using namespace std;
/**********************************************************
 *  Global variables                                      *
 **********************************************************/
struct RoomList		*FirstRoomInList = NULL;
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
	size_t			Len = strlen( Name );

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
	Load the Room database - The format expected is:
	RoomNumber bigint PRIMARY KEY
    ShortDesc VARCHAR(82) NOT NULL
    LongDesc VARCHAR(322) NOT NULL
    north INTEGER NOT NULL
    northeast INTEGER NOT NULL
    east INTEGER NOT NULL
    southeast INTEGER NOT NULL
    south INTEGER NOT NULL
    southwest INTEGER NOT NULL
    west INTEGER NOT NULL
    northwest INTEGER NOT NULL
    up INTEGER NOT NULL
    down INTEGER NOT NULL
 **********************************************************/
bool LoadRoomDatabase( void )
{
	RoomList		*RoomPtr = NULL;
	int				NumFields = 0, Dir = 0;	
	sqlite3			*DatabaseHandle;
	char			*SqliteErrorString = 0;
	int				SqliteReturnCode, RowCount = 0; //set RowCount to Zero
	sqlite3_stmt	*SqlStatement = 0;
	bool			Finished = false;

	SqliteReturnCode = sqlite3_open( ROOM_DATABASE, &DatabaseHandle);
	
	if( SqliteReturnCode ) //if returns anything but SQLITE_OK some kind of error occurred
	{
		ServerLog( "Can't open database: %s\n", sqlite3_errmsg( DatabaseHandle ) );
		sqlite3_close( DatabaseHandle );
		return false;
	}
	/*  compile the sqlite sql statement  */
	SqliteReturnCode = sqlite3_prepare_v2( DatabaseHandle, "select * from rooms", -1, &SqlStatement, NULL );

	if( SqliteReturnCode ) //if returns anything other than SQLITE_OK
	{
		ServerLog( "Error in sqlite3_prepare_v2: %s\n", sqlite3_errmsg( DatabaseHandle ) );
		sqlite3_close( DatabaseHandle );
		return false;
	}

	/* we are ready to execute the prepared statement and start getting rows with sqlite3_step() */
	while( !Finished )
	{
		SqliteReturnCode = sqlite3_step( SqlStatement );

		switch( SqliteReturnCode )
		{
		case SQLITE_DONE:		//there are no more rows
			Finished = true;	//this will break the while loop
			if( RoomPtr )
				RoomPtr->Next = NULL; //terminate the list
			break;

		case SQLITE_ROW:		//we have a row of data
			
			if( RowCount == 0 )	//If this is the first row allocate the first list node
			{
				/*  allocate and prep the Master Room Linked list First Node */
				FirstRoomInList = new RoomList; //allocate first node in master room list
				RoomPtr = FirstRoomInList;		//set the temporary pointer to this newly allocated node
				RoomPtr->Next = NULL;			//terminate the list
			}
			else	//otherwise its not the first node so allocate a new node and move to it
			{
				RoomPtr->Next = new RoomList;	//allocate the next node of the list
				RoomPtr = RoomPtr->Next;		//get the temp pointer incremented
			}

			RoomPtr->Next = NULL;		//make sure the list is terminated before allocating the next node.
			RoomPtr->Value = new Room;	//allocate the Room Object

			/* Load the result row into the Room Object pointed to by RoomPtr->Value */
			RoomPtr->Value->SetRoomNumber( sqlite3_column_int( SqlStatement, 0 ) );
			RoomPtr->Value->SetShortDesc( (char*)sqlite3_column_text( SqlStatement, 1 ) );
			RoomPtr->Value->SetLongDesc( (char *)sqlite3_column_text( SqlStatement, 2 ) );
			RoomPtr->Value->SetExitToRoomNumber( NORTH, sqlite3_column_int( SqlStatement, 3 ) );
			RoomPtr->Value->SetExitToRoomNumber( NORTHEAST, sqlite3_column_int( SqlStatement, 4 ) );
			RoomPtr->Value->SetExitToRoomNumber( EAST, sqlite3_column_int( SqlStatement, 5 ) );
			RoomPtr->Value->SetExitToRoomNumber( SOUTHEAST, sqlite3_column_int( SqlStatement, 6 ) );
			RoomPtr->Value->SetExitToRoomNumber( SOUTH, sqlite3_column_int( SqlStatement, 7 ) );
			RoomPtr->Value->SetExitToRoomNumber( SOUTHWEST, sqlite3_column_int( SqlStatement, 8 ) );
			RoomPtr->Value->SetExitToRoomNumber( WEST, sqlite3_column_int( SqlStatement, 9 ) );
			RoomPtr->Value->SetExitToRoomNumber( NORTHWEST, sqlite3_column_int( SqlStatement, 10 ) );
			RoomPtr->Value->SetExitToRoomNumber( UP, sqlite3_column_int( SqlStatement, 11 ) );
			RoomPtr->Value->SetExitToRoomNumber( DOWN, sqlite3_column_int( SqlStatement, 12 ) );

			RowCount++;		//dont forget to increment the row count.
			break;

		default:
			ServerLog( "Serious issue! hit default case in LoadRoomDatabase()" );
			break;
		}	//end of the switch() block of code

	}	//end of the while loop block of code

	sqlite3_finalize( SqlStatement ); //destroy the compiled sqlite statement and free its memory
	sqlite3_close( DatabaseHandle );	//close the database connection.
	ServerLog( "Loaded %i rooms into memory.", ++RowCount );
	return true;
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