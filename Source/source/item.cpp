/***************************************************************
 *           Minions Mud Server is copyright (C)2001           *
 *                    SonzoSoft Software                       *
 *                                                             *
 *         Programmed by Mark Richardson and David Brown       *
 *															   *
 *			 Mark Richardson	-	sinbaud@hotmail.com	       *
 *		       David Brown	    -	dcbrown73@yahoo.com	       *
 ***************************************************************/
#include <winsock2.h>
#include "item.h"
#include "tcpcomm.h"
#include "sqlite3.h"

/*  Gloabals */
ItemList  *FirstItemInList;

/*  external globals  */

Item::Item()
{
	ItemNumber =  Value = Weight = 0;
	Name[0] = Description[0] = AttackType[0] = '\0';
	MaxDamage = ToHitBonus = Speed = ArmorValue = 0;
	WearLocation = ItemType = 0;
}


bool Item::SetItemName( char *NewName )
{
	if( strlen( NewName ) >= MAX_ITEM_STRING )
	{
		strcpy( Name, "MinionsLengthTooLong" );
		return false;
	}

	strcpy( Name, NewName );
	return true;
}


bool Item::SetItemDesc( char *NewDesc )
{
	if( strlen( NewDesc ) >= MAX_DESC_LENGTH )
	{
		strcpy( Description, "Minions-Length-Too-Long" );
		return false;
	}

	strcpy( Description, NewDesc );
	return true;
}

bool Item::SetAttackType( char *String )
{
	if( strlen( String ) >= MAX_ITEM_STRING )
		return false;

	strcpy( AttackType, String );
	return true;
}

/*=======================================================
Item::SetBonus( int Bonus, int which_bonus ) 

Sets bonus value of item pulled from the database
based on which bonus stat specified.
=======================================================*/
void Item::SetBonus( int Bonus, int which_bonus )
{
	switch ( which_bonus )
	{
	case STRENGTH_BONUS:
		StrengthBonus = Bonus;
		break;
	case AGILITY_BONUS:
		AgilityBonus = Bonus;
		break;
	case HEALTH_BONUS:
		HealthBonus = Bonus;
		break;
	case LUCK_BONUS:
		LuckBonus = Bonus;
		break;
	case WISDOM_BONUS:
		WisdomBonus = Bonus;
		break;
	case HITPOINTS_BONUS:
		HitPointsBonus = Bonus;
		break;
	case MANA_BONUS:
		ManaBonus = Bonus;
		break;
	default:
		ServerLog("Item::SetBonus() hit default with value: %d", which_bonus);
	}
}

/*=======================================================
Item::GetBonus( int which_bonus )

Returns the items bonus modifier for said item
=======================================================*/
int Item::GetBonus( int which_bonus )
{
	switch ( which_bonus )
	{
	case STRENGTH_BONUS:
		return StrengthBonus;
	case AGILITY_BONUS:
		return AgilityBonus;
	case HEALTH_BONUS:
		return HealthBonus;
	case LUCK_BONUS:
		return LuckBonus;
	case WISDOM_BONUS:
		return WisdomBonus;
	case HITPOINTS_BONUS:
		return HitPointsBonus;
	case MANA_BONUS:
		return ManaBonus;
	default:
		ServerLog("Item::SetBonus() hit default with value: %d", which_bonus);
	}
}
/**********************************************************
	Load the Item Database exepcts the following schema:
	ItemNumber int,
	Value int,
	Weight int,
	ItemName VARCHAR(21),
	ItemDesc VARCHAR(201),
	MaxDamage int,
	ToHitBonus int,
	Speed int,
	ArmorValue int,
	AttackType VARCHAR(21),
	MinDamage int
 **********************************************************/
bool LoadItemDatabase( void )
{
	ItemList			*ItemPtr = NULL;	
	sqlite3				*DatabaseHandle;
	char				*SqliteErrorString = 0;
	int					SqliteReturnCode, RowCount = 0; //set RowCount to Zero
	sqlite3_stmt		*SqlStatement = 0;
	bool				Finished = false;

	SqliteReturnCode = sqlite3_open( ITEM_DATABASE, &DatabaseHandle);
	
	if( SqliteReturnCode ) //if returns anything but SQLITE_OK some kind of error occurred
	{
		ServerLog( "Can't open database: %s\n", sqlite3_errmsg( DatabaseHandle ) );
		sqlite3_close( DatabaseHandle );
		return false;
	}
	/*  compile the sqlite sql statement  */
	SqliteReturnCode = sqlite3_prepare_v2( DatabaseHandle, "select * from items", -1, &SqlStatement, NULL );

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
			if( ItemPtr )
				ItemPtr->Next = NULL; //terminate the list
			break;

		case SQLITE_ROW:		//we have a row of data
			
			if( RowCount == 0 )	//If this is the first row allocate the first list node
			{
				/*  allocate and prep the Master Room Linked list First Node */
				FirstItemInList = new ItemList; //allocate first node in master item list
				ItemPtr = FirstItemInList;		//set the temporary pointer to this newly allocated node
				ItemPtr->Next = NULL;			//terminate the list
			}
			else	//otherwise its not the first node so allocate a new node and move to it
			{
				ItemPtr->Next = new ItemList;	//allocate the next node of the list
				ItemPtr = ItemPtr->Next;		//get the temp pointer incremented
			}

			ItemPtr->Next = NULL;		//make sure the list is terminated before allocating the next node.
			ItemPtr->Value = new Item;	//allocate the Item Object

			/* Load the result row into the Item Object pointed to by ItemPtr->Value */
			ItemPtr->Value->SetItemNumber( sqlite3_column_int( SqlStatement, 0 ) );
			ItemPtr->Value->SetItemValue( sqlite3_column_int( SqlStatement, 1 ) );
			ItemPtr->Value->SetItemType( sqlite3_column_int( SqlStatement, 2 ) );
			ItemPtr->Value->SetWeight( sqlite3_column_int( SqlStatement, 3 ) );
			ItemPtr->Value->SetItemName( (char *)sqlite3_column_text( SqlStatement, 4 ) );
			ItemPtr->Value->SetItemDesc( (char *)sqlite3_column_text( SqlStatement, 5 ) );
			ItemPtr->Value->SetMaxDamage( sqlite3_column_int( SqlStatement, 6) );
			ItemPtr->Value->SetToHitBonus( sqlite3_column_int( SqlStatement, 7) );
			ItemPtr->Value->SetSpeed( sqlite3_column_int( SqlStatement, 8) );
			ItemPtr->Value->SetArmorValue( sqlite3_column_int( SqlStatement, 9) );
			ItemPtr->Value->SetAttackType( (char *)sqlite3_column_text( SqlStatement, 10) );
			ItemPtr->Value->SetMinDamage( sqlite3_column_int( SqlStatement, 11) );

			ItemPtr->Value->SetBonus( sqlite3_column_int( SqlStatement, 12), STRENGTH_BONUS );
			ItemPtr->Value->SetBonus( sqlite3_column_int( SqlStatement, 13), AGILITY_BONUS );
			ItemPtr->Value->SetBonus( sqlite3_column_int( SqlStatement, 14), HEALTH_BONUS );
			ItemPtr->Value->SetBonus( sqlite3_column_int( SqlStatement, 15), LUCK_BONUS );
			ItemPtr->Value->SetBonus( sqlite3_column_int( SqlStatement, 16), WISDOM_BONUS );
			ItemPtr->Value->SetBonus( sqlite3_column_int( SqlStatement, 17), HITPOINTS_BONUS  );
			ItemPtr->Value->SetBonus( sqlite3_column_int( SqlStatement, 18), MANA_BONUS );

			ItemPtr->Value->SetWearLocation( sqlite3_column_int( SqlStatement, 19) );

			RowCount++;		//dont forget to increment the row count.
			
			break;

		default:
			ServerLog( "Serious issue! hit default case in LoadItemDatabase()" );
			break;
		}	//end of the switch() block of code

	}	//end of the while loop block of code

	sqlite3_finalize( SqlStatement ); //destroy the compiled sqlite statement and free its memory
	sqlite3_close( DatabaseHandle );	//close the database connection.
	ServerLog( "Loaded %i items into memory.", RowCount );
	return true;
}

Item *SearchForItem( unsigned int ItemNum )
{
	ItemList *ListPtr = '\0';

	for( ListPtr = FirstItemInList; ListPtr && ListPtr->Value; ListPtr = ListPtr->Next )
	{
		if( ListPtr->Value->GetItemNumber() == ItemNum )
			return ListPtr->Value;

	}

	return '\0';
}

