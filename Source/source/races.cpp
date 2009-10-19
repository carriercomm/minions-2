#include <winsock.h>
#include "races.h"
#include "tcpcomm.h"

RaceTable   *MasterRaceTable = '\0';
ClassTable  *MasterClassTable = '\0';

//extern MYSQL	//	SQLConnection;

int LoadRaceTables( void )
{
	RaceTable   *RTable = '\0';
	MasterRaceTable = new RaceTable;
	RTable = MasterRaceTable;
	
	RTable->RaceNumber = 1;
	strcpy( RTable->RaceName, "Dark-Elf" );
	
	RTable->Next = new RaceTable;
	RTable = RTable->Next;
	RTable->RaceNumber = 2;
	strcpy( RTable->RaceName, "Half-Elf" );
	
	RTable->Next = new RaceTable;
	RTable = RTable->Next;
	RTable->RaceNumber = 3;
	strcpy( RTable->RaceName, "Human" );
	
	RTable->Next = new RaceTable;
	RTable = RTable->Next;
	RTable->RaceNumber = 4;
	strcpy( RTable->RaceName, "Djinn" );

	RTable->Next = '\0';

	return 1;
	//MYSQL_RES	*Result;
	//MYSQL_ROW	RowOfData;
	//RaceTable   *RTable = '\0';
	
	//if ( mysql_query( &SQLConnection, "SELECT * FROM races" ) )
	//{
		//ServerLog( "LoadRaceTables(): %s\n", mysql_error( &SQLConnection ) );
		//return 0;
	//}

	//if ( !( Result = mysql_use_result( &SQLConnection ) ) )
	//{
		//ServerLog( "LoadRaceTables(): %s\n", mysql_error( &SQLConnection ) );
		//return 0;
	//}


	//MasterRaceTable = new RaceTable;
	//RTable = MasterRaceTable;

	//RowOfData = mysql_fetch_row( Result );

	//for( ;; )
	//{
		//if( !RowOfData )
			//break;

		//RTable->Next = '\0';
		
		//RTable->RaceNumber = atoi( RowOfData[0] );
		
		//if( RowOfData[1] )
			//strcpy( RTable->RaceName, RowOfData[1] );
	
		//if( !(RowOfData = mysql_fetch_row( Result ) ) )
		//{
			//RTable->Next = '\0';
			//break;
		//}

		//RTable->Next = new RaceTable;
		//RTable = RTable->Next;
	//}

	//return 1;
}

int LoadClassTables( void )
{
	ClassTable  *CTable = '\0';
	
	MasterClassTable = new ClassTable;
	CTable = MasterClassTable;

	CTable->Next = '\0';
	CTable->ClassNumber = 1;
	strcpy( CTable->ClassName, "Druid" );

	CTable->Next = new ClassTable;
	CTable = CTable->Next;
	CTable->ClassNumber = 2;
	strcpy( CTable->ClassName, "Warrior" );

	CTable->Next = new ClassTable;
	CTable = CTable->Next;
	CTable->ClassNumber = 3;
	strcpy( CTable->ClassName, "Paladin" );

	CTable->Next = new ClassTable;
	CTable = CTable->Next;
	CTable->ClassNumber = 4;
	strcpy( CTable->ClassName, "Ranger" );

	CTable->Next = new ClassTable;
	CTable = CTable->Next;
	CTable->ClassNumber = 5;
	strcpy( CTable->ClassName, "Necromancer" );



	CTable->Next = '\0';
	return 1;
	//MYSQL_RES	*Result;
	//MYSQL_ROW	RowOfData;
	//ClassTable  *CTable = '\0';
	
	//if ( mysql_query( &SQLConnection, "SELECT * FROM classes" ) )
	//{
		//ServerLog( "LoadClassTables(): %s\n", mysql_error( &SQLConnection ) );
		//return 0;
	//}

	//if ( !( Result = mysql_use_result( &SQLConnection ) ) )
	//{
		//ServerLog( "LoadClassTables(): %s\n", mysql_error( &SQLConnection ) );
		//return 0;
	//}


	//MasterClassTable = new ClassTable;
	//CTable = MasterClassTable;

	//RowOfData = mysql_fetch_row( Result );

	//for( ;; )
	//{
		//if( !RowOfData )
			//break;

		//CTable->Next = '\0';
		
		//CTable->ClassNumber = atoi( RowOfData[0] );
		
		//if( RowOfData[1] )
			//strcpy( CTable->ClassName, RowOfData[1] );
	
		//if( !(RowOfData = mysql_fetch_row( Result ) ) )
		//{
			//CTable->Next = '\0';
			//break;
		//}

		//CTable->Next = new ClassTable;
		//CTable = CTable->Next;
	//}

	//return 1;
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