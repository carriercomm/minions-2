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
#include <map>
#include <vector>
#include <time.h>
#include <stdlib.h>
#include "direct.h"
#include <fstream>
#include <winsock2.h>
#include "tcpcomm.h"
#include "scheduler.h"
#include "events.h"
#include "combat.h"
#include "room.h"
#include "item.h"
#include "races.h"

using namespace std;

bool	ServerDown;
time_t	CurrentTime, LastTime, BootTime;

extern ofstream		LogFile;


void main( void )
{
	ServerDown = false;
	Room		*TempRoom ='\0';
	Item		*TempItem = '\0';

	/*  throw some credits up on the screen */
	cout<<"\t     *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*\n";
	cout<<"\t     |          Minions Mud Server "<<SERVER_VERSION<<"            |\n";
	cout<<"\t     |                                                   |\n";
	cout<<"\t     |            (C)2009 SonzoSoft Software             |\n";
	cout<<"\t     *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*\n"<<endl;
	
	
 	char curPath[256];
	cout<<"Current working directory is: "<<getcwd(curPath, 256)<<endl;
	/*  open a file for logging */
	cout<<"Opening log files for writing."<<endl;
	LogFile.open( SERVER_LOG_PATH, ios::app );

	if( !LogFile )
	{
		cout<<"Unable to open the server's log file: "<<SERVER_LOG_PATH<<endl;
		return;
	}
	
	/* Try and load the room database */
	cout<<"Loading Room Database."<<endl;
	LoadRoomDatabase();
	cout<<"Fixing room exits."<<endl;
	FixExits();

	/* Try and Load the Item Database  */
	cout<<"Loading Items Database."<<endl;

	if( !( LoadItemDatabase() ) )
		ServerShutDown( true );

	/* Load the Race and Class Tables  */
	cout<<"Loading class and race tables..."<<endl;
	LoadRaceTables();
	LoadClassTables();

	// Load scheduler and add the inital combat event
    scheduler eventScheduler;
	minionsEvent *minionsCombat = new meCombat();
	eventScheduler.pushWaitStack((time(NULL)+4), minionsCombat);

	/* quick hack to put a few items on the floor for testing */
	TempRoom = SearchForRoom( 1 );
	TempItem = SearchForItem( 1 );
	TempRoom->AddItemToRoom( TempItem );
	TempRoom = SearchForRoom( 2 );
	TempItem = SearchForItem( 2 );
	TempRoom->AddItemToRoom( TempItem );
	

	/* Load up the pre-made screens and the winsock DLL  */
	cout<<"Loading ansi screens from disk."<<endl;
	LoadAnsiTitleScreen();
	LoadHelpScreen();
	cout<<"Starting up Winsock."<<endl;
	StartupWinsock();

	/*  Set the BootTime if we need it for some reason */
	cout<<"Setting boot time."<<endl;
	time( &BootTime );
	LastTime = BootTime;

	/*  Seed the random number generator */
	srand((unsigned)time(NULL));
			
	/*  Log the Boot up time */
	ServerLog("Minions Server started at %s", ctime( &BootTime ) );

	/*  Here we go! Start the Main game loop */
	cout<<"Entering main game loop."<<endl;
	cout<<"Minions Server now accepting connections on port: "<<SERVER_PORT<<endl;
	while( !ServerDown )
	{
		/*  Get the current time */
		time( &CurrentTime );
		
//		if( (CurrentTime - LastTime) > 4 )
//		{
//			LastTime = CurrentTime;
//			DoCombatRound();
//		}

		eventScheduler.checkEventsStatus();
		eventScheduler.doEvents(&eventScheduler);


		/*  Check the socket sets and read or write to/from them */
		CheckReadSet();
		CheckWriteSet();
		
	}

	/*  mud_down is true so disconnect everyone and cleanup  */
	ServerShutDown( true );
	

}
