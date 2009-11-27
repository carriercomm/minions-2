#include <winsock.h>
#include <string.h>
#include "races.h"
#include "tcpcomm.h"
#include "sqlite3.h"


RaceTable   *MasterRaceTable = NULL;
ClassTable  *MasterClassTable = NULL;

bool LoadRaceTables( void )
{
	RaceTable		*RTable = NULL;	
	sqlite3			*DatabaseHandle;
	char			*SqliteErrorString = 0;
	int				SqliteReturnCode, RowCount = 0; //set RowCount to Zero
	sqlite3_stmt	*SqlStatement = 0;
	bool			Finished = false;

	SqliteReturnCode = sqlite3_open( RACE_DATABASE, &DatabaseHandle);
	
	if( SqliteReturnCode ) //if returns anything but SQLITE_OK some kind of error occurred
	{
		ServerLog( "Can't open database: %s\n", sqlite3_errmsg( DatabaseHandle ) );
		sqlite3_close( DatabaseHandle );
		return false;
	}
	/*  compile the sqlite sql statement  */
	SqliteReturnCode = sqlite3_prepare_v2( DatabaseHandle, "select * from races", -1, &SqlStatement, NULL );

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
			if( RTable )
				RTable->Next = NULL; //terminate the list
			break;

		case SQLITE_ROW:		//we have a row of data
			
			if( RowCount == 0 )	//If this is the first row allocate the first list node
			{
				/*  allocate and prep the Master Room Linked list First Node */
				MasterRaceTable = new RaceTable; //allocate first node in master race list
				RTable = MasterRaceTable;		//set the temporary pointer to this newly allocated node
			}
			else	//otherwise its not the first node so allocate a new node and move to it
			{
				RTable->Next = new RaceTable;	//allocate the next node of the list
				RTable = RTable->Next;			//get the temp pointer incremented
			}

			RTable->Next = NULL;		//make sure the list is terminated before allocating the next node.
			
			/* Load the result row into the RaceTable entry  */
			RTable->RaceNumber             = sqlite3_column_int( SqlStatement, 0 );
			strcpy( RTable->RaceName,      (char*)sqlite3_column_text( SqlStatement, 1 ) );
			strcpy( RTable->RaceDesc,      (char*)sqlite3_column_text( SqlStatement, 2 ) );
			RTable->RaceStrength           = sqlite3_column_int( SqlStatement, 3 );
			RTable->RaceAgility            = sqlite3_column_int( SqlStatement, 4 );
			RTable->RaceWisdom             = sqlite3_column_int( SqlStatement, 5 );
			RTable->RaceHealth             = sqlite3_column_int( SqlStatement, 6 );

			// HP and Mana are health and wisdom times respective multiplier 
			RTable->RaceMaxHP              = ( RTable->RaceHealth * HP_MULTIPLIER );
			RTable->RaceMaxMana            = ( RTable->RaceWisdom * MANA_MULTIPLIER ); 


			RowCount++;		//dont forget to increment the row count.
			break;

		default:
			ServerLog( "Serious issue! hit default case in LoadRaceTables()" );
			break;
		}	//end of the switch() block of code

	}	//end of the while loop block of code

	sqlite3_finalize( SqlStatement ); //destroy the compiled sqlite statement and free its memory
	sqlite3_close( DatabaseHandle );	//close the database connection.
	ServerLog( "Loaded %i races into memory.", RowCount );
	return true;
}



bool LoadClassTables( void )
{
	ClassTable		*CTable = NULL;	
	sqlite3			*DatabaseHandle;
	char			*SqliteErrorString = 0;
	int				SqliteReturnCode, RowCount = 0; //set RowCount to Zero
	sqlite3_stmt	*SqlStatement = 0;
	bool			Finished = false;

	SqliteReturnCode = sqlite3_open( CLASS_DATABASE, &DatabaseHandle);
	
	if( SqliteReturnCode ) //if returns anything but SQLITE_OK some kind of error occurred
	{
		ServerLog( "Can't open database: %s\n", sqlite3_errmsg( DatabaseHandle ) );
		sqlite3_close( DatabaseHandle );
		return false;
	}
	/*  compile the sqlite sql statement  */
	SqliteReturnCode = sqlite3_prepare_v2( DatabaseHandle, "select * from classes", -1, &SqlStatement, NULL );

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
			if( CTable )
				CTable->Next = NULL; //terminate the list
			break;

		case SQLITE_ROW:		//we have a row of data
			
			if( RowCount == 0 )	//If this is the first row allocate the first list node
			{
				/*  allocate and prep the Master Room Linked list First Node */
				MasterClassTable = new ClassTable; //allocate first node in master race list
				CTable = MasterClassTable;		//set the temporary pointer to this newly allocated node
			}
			else	//otherwise its not the first node so allocate a new node and move to it
			{
				CTable->Next = new ClassTable;	//allocate the next node of the list
				CTable = CTable->Next;			//get the temp pointer incremented
			}

			CTable->Next = NULL;		//make sure the list is terminated before allocating the next node.
			
			/* Load the result row into the RaceTable entry  */
			CTable->ClassNumber = sqlite3_column_int( SqlStatement, 0 );
			strcpy( CTable->ClassName, (char*)sqlite3_column_text( SqlStatement, 1 ) );
			strcpy( CTable->ClassDesc, (char*)sqlite3_column_text( SqlStatement, 2 ) );

			RowCount++;		//dont forget to increment the row count.
			break;

		default:
			ServerLog( "Serious issue! hit default case in LoadClassTables()" );
			break;
		}	//end of the switch() block of code

	}	//end of the while loop block of code

	sqlite3_finalize( SqlStatement ); //destroy the compiled sqlite statement and free its memory
	sqlite3_close( DatabaseHandle );	//close the database connection.
	ServerLog( "Loaded %i classes into memory.", RowCount );
	return true;
}


char *LookupClassStr( int ClassNum )
{
	ClassTable *TempPtr;

	for( TempPtr = MasterClassTable; TempPtr; TempPtr = TempPtr->Next )
	{
		if( TempPtr->ClassNumber == ClassNum )
			return TempPtr->ClassName;
	}

	return "Unknown Class";
}

char *LookupRaceStr( int RaceNum )
{
	RaceTable *TempPtr;

	for( TempPtr = MasterRaceTable; TempPtr; TempPtr = TempPtr->Next )
	{
		if( TempPtr->RaceNumber == RaceNum )
		return TempPtr->RaceName;
	}

	return "Unknown Race";
}