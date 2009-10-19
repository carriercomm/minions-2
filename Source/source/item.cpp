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

/*  Gloabals */
ItemList  *FirstItemInList;

/*  external globals  */

Item::Item()
{
	ItemNumber =  Value = Weight = 0;
	Name[0] = Description[0] = AttackType[0] = '\0';
	MaxDamage = ToHitBonus = Speed = ArmorValue = 0;
}


bool Item::SetItemName( char *NewName )
{
	if( strlen( NewName ) >= MAX_ITEM_STRING )
		return false;

	strcpy( Name, NewName );
	return true;
}


bool Item::SetItemDesc( char *NewDesc )
{
	if( strlen( NewDesc ) >= MAX_DESC_LENGTH )
		return false;

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

/**********************************************************
 *  Functions that are not part of the Item class         *
 **********************************************************/
int LoadItemDatabase( void )
{
	ItemList	*ItemPtr = '\0';
	FirstItemInList = new ItemList;
	ItemPtr = FirstItemInList;

	ItemPtr->Next = '\0';
	ItemPtr->Value = new Item;
	ItemPtr->Value->SetItemNumber( 1 );
	ItemPtr->Value->SetItemValue( 25 );
	ItemPtr->Value->SetWeight( 22 );
	ItemPtr->Value->SetItemName( "cowflop" );
	ItemPtr->Value->SetItemDesc( "This moist, warm cowflop can be used to kill someone." );
	ItemPtr->Value->SetMaxDamage( 29 );
	ItemPtr->Value->SetToHitBonus( 2 );
	ItemPtr->Value->SetSpeed( 15 );
	ItemPtr->Value->SetArmorValue( 5 );
	ItemPtr->Value->SetAttackType( "slop" );
	ItemPtr->Value->SetMinDamage( 1 );

	ItemPtr->Next = new ItemList;
	ItemPtr = ItemPtr->Next;
	ItemPtr->Value = new Item;
	ItemPtr->Value->SetItemNumber( 2 );
	ItemPtr->Value->SetItemValue( 50 );
	ItemPtr->Value->SetWeight( 25 );
	ItemPtr->Value->SetItemName( "diamond tipped whip" );
	ItemPtr->Value->SetItemDesc( "It looks very deadly when wielded by someone who knows how to use it" );
	ItemPtr->Value->SetMaxDamage( 55 );
	ItemPtr->Value->SetToHitBonus( 4 );
	ItemPtr->Value->SetSpeed( 19 );
	ItemPtr->Value->SetArmorValue( 2 );
	ItemPtr->Value->SetAttackType( "whip" );
	ItemPtr->Value->SetMinDamage( 6 );

	ItemPtr->Next = new ItemList;
	ItemPtr = ItemPtr->Next;
	ItemPtr->Value = new Item;
	ItemPtr->Value->SetItemNumber( 3 );
	ItemPtr->Value->SetItemValue( 57 );
	ItemPtr->Value->SetWeight( 30 );
	ItemPtr->Value->SetItemName( "ramrod" );
	ItemPtr->Value->SetItemDesc( "This solid gold ramrod can be used to really stick it to someone." );
	ItemPtr->Value->SetMaxDamage( 60 );
	ItemPtr->Value->SetToHitBonus( 5 );
	ItemPtr->Value->SetSpeed( 33 );
	ItemPtr->Value->SetArmorValue( 7 );
	ItemPtr->Value->SetAttackType( "ram" );
	ItemPtr->Value->SetMinDamage( 10 );

	ItemPtr->Next = new ItemList;
	ItemPtr = ItemPtr->Next;
	ItemPtr->Value = new Item;
	ItemPtr->Value->SetItemNumber( 4 );
	ItemPtr->Value->SetItemValue( 175 );
	ItemPtr->Value->SetWeight( 22 );
	ItemPtr->Value->SetItemName( "golden sword" );
	ItemPtr->Value->SetItemDesc( "This enchanted golden sword packs a punch." );
	ItemPtr->Value->SetMaxDamage( 95 );
	ItemPtr->Value->SetToHitBonus( 8 );
	ItemPtr->Value->SetSpeed( 45 );
	ItemPtr->Value->SetArmorValue( 15 );
	ItemPtr->Value->SetAttackType( "slice" );
	ItemPtr->Value->SetMinDamage( 20 );

	ItemPtr->Next = '\0';
	
	return 1;
	//MYSQL_RES	*Result;
	//MYSQL_ROW	RowOfData;
	///ItemList	*ItemPtr = '\0';
	
	//if ( mysql_query( &SQLConnection, "SELECT * FROM items" ) )
	//{
	//	ServerLog( "LoadItemDatabase: %s\n", mysql_error( &SQLConnection ) );
	//	return 0;
	//}

	//if ( !( Result = mysql_use_result( &SQLConnection ) ) )
	//{
	//	ServerLog( "LoadItemDatabase: %s\n", mysql_error( &SQLConnection ) );
	////	return 0;
	//}


	//FirstItemInList = new ItemList;
	//ItemPtr = FirstItemInList;

	//RowOfData = mysql_fetch_row( Result );

	//for( ;; )
	//{
		//if( !RowOfData )
			//break;

		//ItemPtr->Next = '\0';
		//ItemPtr->Value = new Item;

		//ItemPtr->Value->SetItemNumber( atoi( RowOfData[0] ) );
		
		//if( RowOfData[1] )
			//ItemPtr->Value->SetItemValue( atoi( RowOfData[1] ) );
		//if( RowOfData[2] )
			//ItemPtr->Value->SetWeight( atoi( RowOfData[2] ) );
		
		//ItemPtr->Value->SetItemName( RowOfData[3] );
		//ItemPtr->Value->SetItemDesc( RowOfData[4] );

		//if( RowOfData[5] )
			//ItemPtr->Value->SetMaxDamage( atoi( RowOfData[5] ) );
		//if( RowOfData[6] )
			//ItemPtr->Value->SetToHitBonus( atoi( RowOfData[6] ) );
		//if( RowOfData[7] )
			//ItemPtr->Value->SetSpeed( atoi( RowOfData[7] ) );
		//if( RowOfData[8] )
			//ItemPtr->Value->SetArmorValue( atoi( RowOfData[8] ) );

		//ItemPtr->Value->SetAttackType( RowOfData[9] );

		//if( RowOfData[9] )
			//ItemPtr->Value->SetMinDamage( atoi( RowOfData[8] ) );

		
		//if( !(RowOfData = mysql_fetch_row( Result ) ) )
		//{
			//ItemPtr->Next = '\0';
			//break;
		//}

		//ItemPtr->Next = new ItemList;
		//ItemPtr = ItemPtr->Next;
	//}

	//return 1;
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

