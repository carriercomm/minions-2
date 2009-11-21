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
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "tcpcomm.h"
#include "commands.h"
#include "ansicolor.h"
#include "room.h"
#include "item.h"
#include "scheduler.h"
#include "events.h"
#include "combat.h"

using namespace std;
/**********************************************************
 *  External variables this module uses                   *
 **********************************************************/
extern Connection *PlayerList;
extern scheduler eventScheduler;
extern bool ServerDown;
extern time_t CurrentTime, BootTime;
extern char HelpScreen[MAX_OUTPUT_BUFFER], TitleScreen[MAX_OUTPUT_BUFFER];


/**********************************************************
 *  Commands that are available for use                   *
 **********************************************************/
Command_Table CommandTable[] =
{	
	{"look",		"look      ",   CmdLook,		false },
	{"north",		"north     ",	CmdMoveNorth,	false },
	{"up",			"up        ",	CmdMoveUp,		false },
	{"south",		"south     ",   CmdMoveSouth,	false },
	{"east",		"east      ",	CmdMoveEast,	false },
	{"west",		"west      ",	CmdMoveWest,	false },
	{"ne",			"ne        ",	CmdMoveNE,		false },
	{"se",			"se        ",	CmdMoveSE,		false },
	{"sw",			"sw        ",	CmdMoveSW,		false },
	{"nw",			"nw        ",	CmdMoveNW,		false },
	{"down",		"down      ",	CmdMoveDown,	false },
	{"inventory",	"inventory ",	CmdInventory,	false },
	{"wield",		"wield     ",	CmdWield,		false },
	{"remove",      "remove    ",	CmdRemove,		false },
	{"get",			"get       ",	CmdGetItem,		false },
	{"drop",		"drop      ",	CmdDropItem,	false },
	{"give",		"give      ",	CmdGiveItem,	false },
	{"quit",		"quit      ",	CmdQuit,		false },
	{"emote",		"emote     ",	CmdEmote,		false },
	{"shutdown",	"shutdown  ",	CmdShutDown,	true  },
	{"who",			"who       ",	CmdWho,			false },
	{"credits",		"credits   ",	CmdCredits,		false },
	{"help",		"help      ",	CmdHelp,		false },
	{"attack",		"attack    ",	CmdAttack,		false },
	{"clear",		"clear     ",	CmdClrScr,  	false },
	{"break",		"break     ",	CmdBreakCombat, false },
	{"stat",		"stat      ",   CmdStatus,		false },
	{"telepath",    "telepath  ",	CmdTelepath,	false },
	{"teleport",	"teleport  ",	CmdTeleport,	false },
	{"gossip",		"gossip    ",	CmdGossip,		false },
	{"spawn",		"spawn     ",	CmdSpawn,		true  },
	{"score",       "score     ",   CmdScore,       false },
	{"rest",        "rest      ",   CmdRest,        false },
	{"wear",        "wear      ",   CmdWear,        false },
	//{"sneak",       "sneak     ",   CmdSneak,       false },
	//{"admin",       "admin     ",   CmdAdmin,       false },
	{NULL}
};

/**********************************************************
 *   Function: ParseCommand()                             *
 *                                                        *
 *   tokenize words into up to 3 tokens that are then     *
 *   used to execute the corresponding command            *
 **********************************************************/
void ParseCommand( Connection *Player, char *CmdLine )
{
	// If stun, you can't do anything
	if (FLAG_IS_SET(Player->Flags, FLAG_STUN))
	{
		WriteToBuffer( Player, "%sYou can not do that while stun!\n\r%s", ANSI_BR_CYAN, ANSI_WHITE );
		return;
	}

	/* this is David parse function from the original minions -circa 1998 */
	if( *CmdLine == ' ' || *CmdLine + 1 == '\0' )
	{
		WalkingLook( Player, NULL );
		return;
	}


	if( *CmdLine == '.' )
	{
		Say( Player, CmdLine + 1 );
		return;
	}

	char	*cmdList[5] = { '\0', '\0', '\0', '\0', '\0' }; 
	char	*tempbuf = new char[strlen( CmdLine ) +1];
	int		count=0, pos=0,len=0;
		
	
	while(CmdLine[pos]!='\0' && count < 5 )
	{
		
		if( CmdLine[pos] ==' ' )
		{
			tempbuf[pos]='\0';
			pos++;
		}

		if( CmdLine[pos] !=' ' && CmdLine[pos] != '\0')
		{
		    cmdList[count]=&tempbuf[pos];
			count++;
	    	
			while( CmdLine[pos] != ' ' && CmdLine[pos] != '\0')
			{
		    	tempbuf[pos] = CmdLine[pos];
				pos++;
			}
			tempbuf[pos]='\0';
		}
	}
	/* end David's parse code from original minions */
	len = strlen( cmdList[0] );
	
	for( int loop = 0; CommandTable[loop].Command != NULL; loop++ )
	{
		if( !strnicmp( CommandTable[loop].Command, cmdList[0], len ) ) //if command matches a real command
		{
			(*CommandTable[loop].FuncPtr )( Player, CmdLine, cmdList[1], cmdList[2] ); //execute command

			/* added the status prompt code here  */
			if ( strnicmp(cmdList[0], "quit", 4) )
				WriteToBuffer( Player, "%s%s[ HP: %i] > %s", ANSI_CLR_SOL, ANSI_BR_CYAN, Player->Player.GetHitPoints(), ANSI_WHITE );

			return;
		}

	}
	WriteToBuffer( Player, "%s***  Your command had no effect. ***%s\n\r",
		ANSI_BR_RED, ANSI_WHITE );

	WriteToBuffer( Player, "%s%s[ HP: %i] > %s", ANSI_CLR_SOL, ANSI_BR_CYAN, Player->Player.GetHitPoints(), ANSI_WHITE );

	delete tempbuf;

	return;
}

/**********************************************************
 *  Function: Say()                                       *
 *                                                        *
 *  Outputs a message to the room the player is currently *
 *  located in                                            *
 **********************************************************/
void Say( Connection *Conn, char *Text )
{
	Room		*TempRom = '\0';
	PlayersInRoom *ListPtr = '\0';

	TempRom = Conn->Player.GetRoom();

	if( !TempRom )
	{
		WriteToBuffer( Conn, "Internal server error 90332\n\r" );
		return;
	}

	ListPtr = TempRom->GetFirstPlayer();

	for( ListPtr = TempRom->GetFirstPlayer(); ListPtr; ListPtr = ListPtr->Next )
	{
		if( ListPtr->Player == Conn )
		{
			WriteToBuffer( Conn, "%s%sYou say: %s%s\n\r", ANSI_CLR_SOL, ANSI_BR_GREEN, ANSI_WHITE, Text );
			WriteToBuffer( Conn, "%s%s[ HP: %i] > %s", ANSI_CLR_SOL, ANSI_BR_CYAN, Conn->Player.GetHitPoints(), ANSI_WHITE );
			continue;
		}

		if( ListPtr->Player->Status == STATUS_AUTHENTICATED )
		{
			WriteToBuffer( ListPtr->Player, "%s%s%s says: %s%s\n\r", ANSI_CLR_SOL, ANSI_BR_GREEN,
		                Conn->Player.GetFirstName(), ANSI_WHITE, Text );
			WriteToBuffer( ListPtr->Player, "%s%s[ HP: %i] > %s", ANSI_CLR_SOL, ANSI_BR_CYAN, ListPtr->Player->Player.GetHitPoints(), ANSI_WHITE );
		}
	}

	return;
}

/**********************************************************
 *  Function CmdQuit()                                    *
 *                                                        *
 *  Disconnects the player from the server                *
 **********************************************************/
COMMAND(Quit)
{
	Disconnect( Player );
	return;
}

/**********************************************************
 *  Function: CmdCredits()                                *
 *                                                        *
 *  Shameless plug command                                *
 **********************************************************/
COMMAND(Credits)
{
	if( Player )
	{
		//WriteToBuffer( Player, "%s-------[ Minions Mud Server - %s ]------------\n\r\t    %s(C)1998 - 2009 SonzoSoft%s\n\rProgramming: Mark Richardson - sinbaud@hotmail.com\n\r\t\tAnd David Brown.\n\r-----------------------------------------------------\n\r", ANSI_BR_RED, SERVER_VERSION, ANSI_WHITE, ANSI_BR_RED );
		DisplayFile( Player, HelpScreen );
		WriteToBuffer( Player, "\n\r" );
		WriteToBuffer( Player, "%s%s[ HP: %i] > %s", ANSI_CLR_SOL, ANSI_BR_CYAN, Player->Player.GetHitPoints(), ANSI_WHITE );
		return;
	}

}


/**********************************************************
 *   Function: CmdShutDown()                              *
 *                                                        *
 *   Shuts down the entire server                         *
 **********************************************************/
COMMAND(ShutDown)
{
	if( !Argument )
	{
		WriteToBuffer( Player, "%sUsage:%s shutdown <password>\n\r",
			ANSI_BR_RED, ANSI_WHITE );
		return;
	}

	if( strcmp( Argument, SHUTDOWN_PASSWORD ) )
	{
		WriteToBuffer( Player, "%sIncorrect shutdown password.%s\n\r",
			ANSI_BR_RED, ANSI_WHITE );
		return;
	}
	
	/*  Log the Shut Down time */
	ServerLog("Minions Server shutdown remotely by %s at %s\n", Player->Player.GetFirstName(), ctime( &CurrentTime ) );

	ServerDown = true;
	return;
}

/**********************************************************
 *   Function: CmdWho()                                   *
 *                                                        *
 *  Gives the requester a list of everyone that is online *
 **********************************************************/
COMMAND(Who)
{
	Connection *TempConn;
	time_t UpTime;
	char TimeString[80];

	UpTime = CurrentTime - BootTime;

	if( UpTime <= 60 )
	{
		strcpy( TimeString, "seconds" );
	}

	else if ( UpTime > 950400 )
	{
		UpTime = UpTime / 864000;
		strcpy( TimeString, "days" );
	}

	else if( UpTime > 7200 )
	{
		UpTime = UpTime / 3600;
		strcpy( TimeString, "hours" );
	}

	else if( UpTime > 60 )
	{
		UpTime = UpTime / 60;
		strcpy( TimeString, "minutes" );
	}

	WriteToBuffer( Player, "\n\r%sCurrent Players of Minions Mud\n\r", ANSI_BR_GREEN );
	WriteToBuffer( Player, "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n\r%s",
		ANSI_BR_YELLOW );

	for( TempConn = PlayerList; TempConn; TempConn = TempConn->Next )
	{
		WriteToBuffer( Player, "%s\t%s\t%s\t%s\t\t%s\n\r", TempConn->Player.GetFirstName(), TempConn->Player.GetRaceStr(),
			TempConn->Player.GetClassStr(), TempConn->Player.GetIpAddress(), TempConn->Player.GetHostName() );
	}
		
	WriteToBuffer( Player, "%s=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n\r",
		ANSI_BR_GREEN );

	WriteToBuffer( Player, "Server uptime in %s: %i\n\r", TimeString, UpTime );
	WriteToBuffer( Player, "%s%s[ HP: %i] > %s", ANSI_CLR_SOL, ANSI_BR_CYAN, Player->Player.GetHitPoints(), ANSI_WHITE );
	return;
}

COMMAND(Score)
{
	Connection *TempConn;
	char TimeString[80];
	time_t UpTime;

	UpTime = CurrentTime - BootTime;
	
	if( UpTime <= 60 )
	{
		strcpy( TimeString, "seconds" );
	}

	else if ( UpTime > 950400 )
	{
		UpTime = UpTime / 864000;
		strcpy( TimeString, "days" );
	}

	else if( UpTime > 7200 )
	{
		UpTime = UpTime / 3600;
		strcpy( TimeString, "hours" );
	}

	else if( UpTime > 60 )
	{
		UpTime = UpTime / 60;
		strcpy( TimeString, "minutes" );
	}

	WriteToBuffer( Player, "\n\r%sCurrent Scores in Minions Mud\n\r",
		ANSI_BR_GREEN );
	WriteToBuffer( Player, "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n\r%s",
		ANSI_BR_YELLOW );

	for( TempConn = PlayerList; TempConn; TempConn = TempConn->Next )
	{
		WriteToBuffer( Player, "%s has %d kills and has been killed %d times.\n\r", TempConn->Player.GetFirstName(),
			TempConn->Player.GetKills(), TempConn->Player.GetBeenKilled() );
	}
		
	WriteToBuffer( Player, "%s=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n\r",
		ANSI_BR_GREEN );

	WriteToBuffer( Player, "Server uptime in %s: %i\n\r", TimeString, UpTime );
	WriteToBuffer( Player, "%s%s[ HP: %i] > %s", ANSI_CLR_SOL, ANSI_BR_CYAN, Player->Player.GetHitPoints(), ANSI_WHITE );
	return;
}

/**********************************************************
 *   Function: CmdHelp()                                  *
 *                                                        *
 *   Iterates thru the CommandList and gives list of      *
 *   the commands available to the requester              *
 **********************************************************/
COMMAND(Help)
{
	WriteToBuffer( Player, "%s\t\tList of available commands%s\n\r",
		ANSI_BR_YELLOW, ANSI_BR_CYAN );
	
	for( int loop = 0; CommandTable[loop].Command != NULL; loop++ )
	{
		if( loop % 5 == 0)		//break to the next line
			WriteToBuffer( Player, "\n\r" );

		/* If its an administrative command flag it as so  */
		if( CommandTable[loop].AdminOnly == true )
			WriteToBuffer( Player, "%s%s%s ", ANSI_BR_YELLOW, CommandTable[loop].Padded, ANSI_BR_CYAN );
		else
			WriteToBuffer( Player, "%s  ", CommandTable[loop].Padded );
	}
	
	WriteToBuffer( Player, "%s%s[ HP: %i] > %s", ANSI_CLR_SOL, ANSI_BR_CYAN, Player->Player.GetHitPoints(), ANSI_WHITE );

	return;
}

/**********************************************************
 *  Attack command                                        *
 **********************************************************/
COMMAND(Attack)
{
	Connection	*Victim = '\0';
	Room		*TempRoom = '\0';

	// Can't be resting if you're even attempting to attack!
	Player->Player.SetRestingStatus(NOT_RESTING);

	if( Argument == '\0' )
	{
		WriteToBuffer( Player, "Usage: bs <target>\n\r" );
		return;
	}

	Victim = SearchForPlayer( Argument );

	if( !Victim )
	{
		WriteToBuffer( Player, "You don't see %s here.\n\r", Argument );
		return;
	}
	// If being attacked, you can't rest.
	Victim->Player.SetRestingStatus(NOT_RESTING);
	if( Player == Victim )
	{
		WriteToBuffer( Player, "%sAre you a masochist or what?%s\n\r",
			ANSI_BR_YELLOW, ANSI_WHITE );
		return;
	}

	if( Player->Player.GetRoom() != Victim->Player.GetRoom() )
	{
		WriteToBuffer( Player, "%sYou don't see %s here.%s\n\r",
			ANSI_BR_RED, Argument, ANSI_WHITE );
		return;
	}
	if (Player->Player.GetAttackEvent())
		StopCombat(Player);
    Player->Victim = Victim;
	Victim->Player.SetRestingStatus(NOT_RESTING);
	// Tell players combat started and create combat event
	DisplayCombatStatus(Player, true);
	minionsEvent *meleeEvent = new meMelee(Player, Victim);
	Player->Player.SetAttackEvent(meleeEvent);
	eventScheduler.pushCombatStack(meleeEvent);

	return;
}

/**********************************************************
 *  Clear the screen command                              *
 **********************************************************/
COMMAND(ClrScr)
{
	WriteToBuffer( Player, "%s%s[ HP: %i] > %s", ANSI_CLR_SCR, ANSI_BR_CYAN, Player->Player.GetHitPoints(), ANSI_WHITE );
	return;
}

/**********************************************************
 *  Break combat command                                  *
 **********************************************************/
COMMAND(BreakCombat)
{
	minionsEvent *thisEvent;
	if( !Player->Victim )
	{
		WriteToBuffer( Player, "%sYou are not engaged in combat!%s\n\r",
			ANSI_BR_RED, ANSI_WHITE );
		return;
	}

	
	WriteToBuffer( Player, "%s*** You break combat ***%s\n\r", ANSI_YELLOW,
		ANSI_WHITE );

	WriteToBuffer( Player->Victim, "%s*** %s stops attacking you ***%s\n\r",
		ANSI_YELLOW, Player->Player.GetFirstName(), ANSI_WHITE );

	StopCombat(Player);
	//Player->Victim = '\0';
}

/**********************************************************
 *   Status of the players character					  *
 **********************************************************/
COMMAND(Status)
{
	WriteToBuffer( Player, "%sName:%s %s %s    %sLevel:%s %d     %sExp: %s%d\n\r%sHit Points:%s %d of %d     %sMana:%s %d of %d\n\r",
		ANSI_BR_GREEN, ANSI_WHITE, Player->Player.GetFirstName(), Player->Player.GetLastName(),
		ANSI_BR_GREEN, ANSI_WHITE, Player->Player.GetLevel(), ANSI_BR_GREEN, ANSI_WHITE, Player->Player.GetExp(),
		ANSI_BR_GREEN, ANSI_WHITE, Player->Player.GetHitPoints(), Player->Player.GetMaxHitPoints(),
		ANSI_BR_GREEN, ANSI_WHITE, Player->Player.GetMana(), Player->Player.GetMaxMana() );

	WriteToBuffer( Player, "%sArmor Class: %s%d          %sTHAC0: %s%d\n\r",
		ANSI_BR_GREEN, ANSI_WHITE, Player->Player.GetArmorClass(), ANSI_BR_GREEN,
		ANSI_WHITE, Player->Player.GetTHAC0() );
	
	WriteToBuffer( Player, "%sStrength: %s%d          %sHealth: %s%d\n\r%sAgility:  %s%d          %sLuck:   %s%d\n\r%sWisdom:   %s%d          %sWeight:   %s%d\n\r",
		ANSI_BR_GREEN, ANSI_WHITE, Player->Player.GetStrength(), ANSI_BR_GREEN, ANSI_WHITE, Player->Player.GetHealth(), ANSI_BR_GREEN,
		ANSI_WHITE, Player->Player.GetAgility(), ANSI_BR_GREEN, ANSI_WHITE, Player->Player.GetLuck(), ANSI_BR_GREEN, ANSI_WHITE,
		Player->Player.GetWisdom(), ANSI_BR_GREEN, ANSI_WHITE, Player->Player.GetPlayerWeight() );
	return;
}

/**********************************************************
 *  Telepath command                                      *
 **********************************************************/
COMMAND(Telepath)
{
	char		*TelepathArg = '\0';
	Connection	*Receiver = '\0';
	int			Count = 0;

	if( !Argument )
	{
		if( FLAG_IS_SET( Player->Flags, FLAG_TELEPATHS ) )
		{
			REMOVE_PFLAG( Player->Flags, FLAG_TELEPATHS );
			WriteToBuffer( Player, "%sTelepaths are now turned off%s\n\r",
			ANSI_BR_CYAN, ANSI_WHITE );
		}

		else
		{
			SET_PFLAG( Player->Flags, FLAG_TELEPATHS );
			WriteToBuffer( Player, "%sTelepaths are now turned on.%s\n\r",
				ANSI_BR_CYAN, ANSI_WHITE );
		}
		
		return;
	}

	/*  if you have telepaths off you cannot telepath!  */
	if( !FLAG_IS_SET( Player->Flags, FLAG_TELEPATHS ) )
	{
		WriteToBuffer( Player, "%sYour telepaths are disabled. Re-enable them if you would like to telepath someone!%s\n\r",
			ANSI_BR_CYAN, ANSI_WHITE );
		return;
	}

	Receiver = SearchForPlayer( Argument );

	if( !Receiver )
	{
		WriteToBuffer( Player, "%s%s is not online at the moment!%s\n\r",
			ANSI_BR_CYAN, Argument, ANSI_WHITE );
		
		return;
	}

	if( !FLAG_IS_SET( Receiver->Flags, FLAG_TELEPATHS ) )
	{
		WriteToBuffer( Player, "%s%s has thier telepaths turned off.%s\n\r",
			ANSI_BR_CYAN, Receiver->Player.GetFirstName(), ANSI_WHITE );
		return;
	}

	TelepathArg = FullCmd;

	while( *TelepathArg && Count < 2 )
	{
		if( *TelepathArg == ' ' )
			Count++;

		TelepathArg++;
	}
	

	WriteToBuffer( Receiver, "%s%s telepaths:%s %s\n\r", ANSI_BR_CYAN,
		Player->Player.GetFirstName(), ANSI_WHITE, TelepathArg );

	WriteToBuffer( Player, "%s*** Telepath sent to %s ***%s\n\r",
		ANSI_BR_CYAN, Receiver->Player.GetFirstName(), ANSI_WHITE );

	return;
}
	
/**********************************************************
 *  Gossip command                                        *
 **********************************************************/
COMMAND(Gossip)
{
	char			*GossipArg = '\0';
	Connection		*TempConn = '\0';

	if( !Argument )
	{
		if( FLAG_IS_SET( Player->Flags, FLAG_GOSSIPS ) )
		{
			REMOVE_PFLAG( Player->Flags, FLAG_GOSSIPS );
			WriteToBuffer( Player, "%sGossips are now turned off%s\n\r",
			ANSI_BR_CYAN, ANSI_WHITE );
		}

		else
		{
			SET_PFLAG( Player->Flags, FLAG_GOSSIPS );
			WriteToBuffer( Player, "%sGossips are now turned on.%s\n\r",
				ANSI_BR_CYAN, ANSI_WHITE );
		}
		
		return;
	}

	/*  if you have gossips off you cannot gossip!  */
	if( !FLAG_IS_SET( Player->Flags, FLAG_GOSSIPS ) )
	{
		WriteToBuffer( Player, "%sYour Gossips are disabled. Re-enable them if you would like to gossip!%s\n\r", ANSI_BR_MAGENTA, ANSI_WHITE );
		return;
	}

	GossipArg = FullCmd;

	while( *GossipArg )
	{
		if( *GossipArg == ' ' )
			break;

		GossipArg++;
	}

	GossipArg++;

	/* send it on out to all connections */
	for( TempConn = PlayerList; TempConn; TempConn = TempConn->Next )
	{
		if( TempConn->Status == STATUS_AUTHENTICATED && FLAG_IS_SET( TempConn->Flags, FLAG_GOSSIPS ) )
		
			WriteToBuffer( TempConn, "%s%s gossips: %s%s\n\r", ANSI_BR_MAGENTA,
			      Player->Player.GetFirstName(), ANSI_WHITE, GossipArg );
	}

	return;
}

/**********************************************************
 *  Look Command                                          *
 **********************************************************/
COMMAND(Look)
{
	Room			*TempRom = '\0';
	Room			*LookIntoRoom = '\0';
	Item			*TempItem ='\0';
	ItemsInRoom		*ItemList = '\0';
	Connection		*Victim = '\0';
	char			*Direction ='\0';
	char			*FromDir = '\0';

	/*  get the room pointer we'll be dealing with */
	TempRom = Player->Player.GetRoom();

	if( !TempRom )
	{
		WriteToBuffer( Player, "NULL Room pointer in CmdLook()\n\r" );
		return;
	}

	/* if there is an argument, figure out what they want to look at */
	if( Argument )
	{
	
		/* first try all the people in the room */
		Victim = TempRom->SearchRoomForPlayer( Argument );
		if( Victim )
		{
			WriteToBuffer( Player, "%s", Victim->Player.GetDescription() );
			
			WriteToBuffer( Victim, "%s%s looks you over.%s\n\r", ANSI_BR_BLUE,
				Player->Player.GetFirstName(), ANSI_WHITE );

			TempRom->SelectiveBroadcast( Player, Victim, "%s%s looks %s over.%s\n\r",
				ANSI_BR_BLUE, Player->Player.GetFirstName(), Victim->Player.GetFirstName(),
				ANSI_WHITE );

			return;
		}

		/*  then try all the items in the room */
		TempItem = TempRom->SearchRoomForItem( Argument );
		if( TempItem )
		{
			WriteToBuffer( Player, "%s[%s]%s\n\r%s\n\r", ANSI_BR_CYAN,
				TempItem->GetItemName(), ANSI_WHITE, TempItem->GetItemDesc() );

			TempRom->SelectiveBroadcast( Player, NULL, "%s%s inspects the %s.%s\n\r",
				ANSI_BR_BLUE, Player->Player.GetFirstName(), TempItem->GetItemName(),
				ANSI_WHITE );

			return;
		}

		/* then try all the items the player is holding */
		TempItem = Player->Player.SearchPlayerForItem( Argument );
		if( TempItem )
		{
			WriteToBuffer( Player, "%s[%s]%s\n\r%s\n\r", ANSI_BR_CYAN,
				TempItem->GetItemName(), ANSI_WHITE, TempItem->GetItemDesc() );

			TempRom->SelectiveBroadcast( Player, NULL, "%s%s inspects his %s.%s\n\r",
				ANSI_BR_BLUE, Player->Player.GetFirstName(), TempItem->GetItemName(),
				ANSI_WHITE );

			return;
		}

		/* then try looking into an adjacent room  */
		char *Ch = Argument + 1;
		
		switch( *Argument )
		{
		case 'n':
			if( *Ch == 'e' )
			{
				LookIntoRoom = TempRom->GetExit( NORTHEAST );
				Direction = "northeast";
				FromDir = "southwest";
				break;
			}
		
			else if( *Ch == 'w' )
			{
				LookIntoRoom = TempRom->GetExit( NORTHWEST );
				Direction = "northwest";
				FromDir = "southeast";
				break;
			}

			LookIntoRoom = TempRom->GetExit( NORTH );
			Direction = "north";
			FromDir = "south";
			break;
		
		case 's':
			if( *Ch == 'e' )
			{
				LookIntoRoom = TempRom->GetExit( SOUTHEAST );
				Direction = "southeast";
				FromDir = "northwest";
				break;
			}

			else if( *Ch == 'w' )
			{
				LookIntoRoom = TempRom->GetExit( SOUTHWEST );
				Direction = "southwest";
				FromDir = "northeast";
				break;
			}

			LookIntoRoom = TempRom->GetExit( SOUTH );
			Direction = "south";
			FromDir = "north";
			break;
		
		case 'e':
			LookIntoRoom = TempRom->GetExit( EAST );
			Direction = "east";
			FromDir = "west";
			break;
		
		case 'w':
			LookIntoRoom = TempRom->GetExit( WEST );
			Direction = "west";
			FromDir = "east";
			break;
		
		case 'u':
			LookIntoRoom = TempRom->GetExit( UP );
			Direction = "up";
			FromDir = "below";
			break;
		
		case 'd':
			LookIntoRoom = TempRom->GetExit( DOWN );
			Direction = "down";
			FromDir = "above";
			break;
		
		default:
			LookIntoRoom = '\0';
			break;
		}

		if( LookIntoRoom )
		{
			WalkingLook( Player, LookIntoRoom );
			
			/* tell the room your in that your looking */
			TempRom->SelectiveBroadcast( Player, NULL, "%s%s looks %s.%s\n\r",
				ANSI_BR_YELLOW, Player->Player.GetFirstName(), Direction, ANSI_WHITE );
			
			/*  tell the room your looking into that your looking */
			LookIntoRoom->BroadcastRoom( "%s%s peeks into the room from the %s!%s\n\r",
				ANSI_YELLOW, Player->Player.GetFirstName(), FromDir, ANSI_WHITE );

			return;
		}
	
		/*  If nothing matches tell them about it and bail out */
		WriteToBuffer( Player, "%sYou don't see %s around here!%s\n\r",
			ANSI_BR_RED, Argument, ANSI_WHITE );

		return;
	}

				
	/* if no argument just ouput the room description */
	WriteToBuffer( Player, "\n\r%s%s\n\r%s%s\n\r", ANSI_BR_GREEN, TempRom->GetShortDesc(),
		ANSI_WHITE, TempRom->GetLongDesc() );

	if( TempRom->GetItemCount() > 0 )
	{
		ItemList = TempRom->GetFirstItem();
		
		if( !ItemList )
		{
			WriteToBuffer( Player, "Getting items on floor messed up!\n\r" );
			return;
		}
		
		WriteToBuffer( Player, "%sYou notice: ", ANSI_BR_CYAN );
		
		for(int Count = 0 ; ItemList; ItemList = ItemList->Next )
		{
			if( Count == 0 )
				WriteToBuffer( Player, "%s", ItemList->Item->GetItemName() );
			else
				WriteToBuffer( Player, ", %s", ItemList->Item->GetItemName() );
			
			Count++;
		}

		WriteToBuffer( Player, "%s\n\r", ANSI_WHITE );
	}

	if( TempRom->GetPlayerCount() > 1 )
	{
		int Count = 0;
		PlayersInRoom	*Temp = '\0';

		Temp = TempRom->GetFirstPlayer();

		if (!Temp)
			return;

		WriteToBuffer( Player, "%sAlso here: %s", ANSI_BR_GREEN, ANSI_BR_MAGENTA );
		
		for( ; Temp; Temp = Temp->Next )
		{
			if( Temp->Player == Player )
				continue;

			if( Count < 1 )
			{
				WriteToBuffer( Player, "%s", Temp->Player->Player.GetFirstName() );
				Count++;
			}

			else
			{
				WriteToBuffer( Player, ", %s", Temp->Player->Player.GetFirstName() );
			}
		}

		WriteToBuffer( Player, "%s\n\r", ANSI_WHITE );
	}

	WriteToBuffer( Player, "%sObvious exits: %s%s\n\r", ANSI_BR_YELLOW,
		TempRom->GetExitString(), ANSI_WHITE );

	TempRom->SelectiveBroadcast( Player, NULL, "%s%s looks around the room.%s\n\r",
		ANSI_BR_YELLOW, Player->Player.GetFirstName(), ANSI_WHITE );
	
	return;
}

COMMAND(MoveNorth)
{
	Room	*RoomPtr = '\0';
	Room	*NewRoom = '\0';

	RoomPtr = Player->Player.GetRoom();

	if( !RoomPtr )
	{
		WriteToBuffer( Player, "There was an error\n\r" );
		return;
	}

	//Can't rest if you're trying to move
	Player->Player.SetRestingStatus(NOT_RESTING);

	NewRoom = RoomPtr->GetExit( NORTH );

	if( !NewRoom )
	{
		WriteToBuffer( Player, "%sThere is no exit to the north!%s\n\r",
			ANSI_BR_RED, ANSI_WHITE);
		
		RoomPtr->SelectiveBroadcast( Player, NULL, "%s%s runs into the northern wall!%s\n\r",
			ANSI_BR_YELLOW, Player->Player.GetFirstName(), ANSI_WHITE );

		return;
	}

	RoomPtr->RemovePlayerFromRoom( Player );
	RoomPtr->BroadcastRoom( "%s%s leaves to the north.%s\n\r", ANSI_BR_GREEN, Player->Player.GetFirstName(), ANSI_WHITE );
	NewRoom->BroadcastRoom( "%s%s walks in from the south%s\n\r", ANSI_BR_GREEN, Player->Player.GetFirstName(), ANSI_WHITE );
	NewRoom->AddPlayerToRoom( Player );
	Player->Player.SetRoom( NewRoom );
	WalkingLook( Player, NewRoom );
	
	return;
}



COMMAND(MoveSouth)
{
	Room	*RoomPtr = '\0';
	Room	*NewRoom = '\0';

	RoomPtr = Player->Player.GetRoom();

	if( !RoomPtr )
	{
		WriteToBuffer( Player, "There was an error\n\r" );
		return;
	}

	//Can't rest if you're trying to move
	Player->Player.SetRestingStatus(NOT_RESTING);

	NewRoom = RoomPtr->GetExit( SOUTH );

	if( !NewRoom )
	{
		WriteToBuffer( Player, "%sThere is no exit to the south!%s\n\r",
			ANSI_BR_RED, ANSI_WHITE );

		RoomPtr->SelectiveBroadcast( Player, NULL, "%s%s runs into the southern wall!%s\n\r",
			ANSI_BR_YELLOW, Player->Player.GetFirstName(), ANSI_WHITE );
		
		return;
	}

	RoomPtr->RemovePlayerFromRoom( Player );
	RoomPtr->BroadcastRoom( "%s%s leaves to the south.%s\n\r", 
		ANSI_BR_GREEN, Player->Player.GetFirstName(), ANSI_WHITE );
	NewRoom->BroadcastRoom( "%s%s walks in from the north%s\n\r",
		ANSI_BR_GREEN, Player->Player.GetFirstName(), ANSI_WHITE );
	NewRoom->AddPlayerToRoom( Player );
	Player->Player.SetRoom( NewRoom );
	WalkingLook( Player, NewRoom );
	
	return;

}

COMMAND(MoveUp)
{
	Room	*RoomPtr = '\0';
	Room	*NewRoom = '\0';

	RoomPtr = Player->Player.GetRoom();

	if( !RoomPtr )
	{
		WriteToBuffer( Player, "There was an error\n\r" );
		return;
	}

	//Can't rest if you're trying to move
	Player->Player.SetRestingStatus(NOT_RESTING);

	NewRoom = RoomPtr->GetExit( UP );

	if( !NewRoom )
	{
		WriteToBuffer( Player, "%sThere is no upward exit!%s\n\r",
			ANSI_BR_RED, ANSI_WHITE );

		RoomPtr->SelectiveBroadcast( Player, NULL, "%s%s runs into the upward wall!%s\n\r",
			ANSI_BR_YELLOW, Player->Player.GetFirstName(), ANSI_WHITE );

		return;
	}

	RoomPtr->RemovePlayerFromRoom( Player );
	RoomPtr->BroadcastRoom( "%s%s leaves upward.%s\n\r",
		ANSI_BR_GREEN, Player->Player.GetFirstName(), ANSI_WHITE );
	NewRoom->BroadcastRoom( "%s%s enters from below%s\n\r",
		ANSI_BR_GREEN, Player->Player.GetFirstName(), ANSI_WHITE );
	NewRoom->AddPlayerToRoom( Player );
	Player->Player.SetRoom( NewRoom );
	WalkingLook( Player, NewRoom );
	return;

}

COMMAND(MoveDown)
{
	Room	*RoomPtr = '\0';
	Room	*NewRoom = '\0';

	RoomPtr = Player->Player.GetRoom();

	if( !RoomPtr )
	{
		WriteToBuffer( Player, "There was an error\n\r" );
		return;
	}

	//Can't rest if you're trying to move
	Player->Player.SetRestingStatus(NOT_RESTING);

	NewRoom = RoomPtr->GetExit( DOWN );

	if( !NewRoom )
	{
		WriteToBuffer( Player, "%sThere is no downward exit!%s\n\r",
			ANSI_BR_RED, ANSI_WHITE );

		RoomPtr->SelectiveBroadcast( Player, NULL, "%s%s runs into the downward wall!%s\n\r",
			ANSI_BR_YELLOW, Player->Player.GetFirstName(), ANSI_WHITE );

		return;
	}

	RoomPtr->RemovePlayerFromRoom( Player );
	RoomPtr->BroadcastRoom( "%s%s leaves downward.%s\n\r", 
		ANSI_BR_GREEN, Player->Player.GetFirstName(), ANSI_WHITE );
	NewRoom->BroadcastRoom( "%s%s enters from above%s\n\r",
		ANSI_BR_GREEN, Player->Player.GetFirstName(), ANSI_WHITE );
	NewRoom->AddPlayerToRoom( Player );
	Player->Player.SetRoom( NewRoom );
	WalkingLook( Player, NewRoom );
	return;

}

COMMAND(MoveEast)
{
	Room	*RoomPtr = '\0';
	Room	*NewRoom = '\0';

	RoomPtr = Player->Player.GetRoom();

	if( !RoomPtr )
	{
		WriteToBuffer( Player, "There was an error\n\r" );
		return;
	}

	//Can't rest if you're trying to move
	Player->Player.SetRestingStatus(NOT_RESTING);

	NewRoom = RoomPtr->GetExit( EAST );

	if( !NewRoom )
	{
		WriteToBuffer( Player, "%sThere is no exit to the east!%s\n\r",
			ANSI_BR_RED, ANSI_WHITE);

		RoomPtr->SelectiveBroadcast( Player, NULL, "%s%s runs into the eastern wall!%s\n\r",
			ANSI_BR_YELLOW, Player->Player.GetFirstName(), ANSI_WHITE );

		return;
	}

	RoomPtr->RemovePlayerFromRoom( Player );
	RoomPtr->BroadcastRoom( "%s%s leaves to the east.%s\n\r", ANSI_BR_GREEN, Player->Player.GetFirstName(), ANSI_WHITE );
	NewRoom->BroadcastRoom( "%s%s walks in from the west%s\n\r", ANSI_BR_GREEN, Player->Player.GetFirstName(), ANSI_WHITE );
	NewRoom->AddPlayerToRoom( Player );
	Player->Player.SetRoom( NewRoom );
	WalkingLook( Player, NewRoom );
	
	return;
}


COMMAND(MoveWest)
{
	Room	*RoomPtr = '\0';
	Room	*NewRoom = '\0';

	RoomPtr = Player->Player.GetRoom();

	if( !RoomPtr )
	{
		WriteToBuffer( Player, "There was an error\n\r" );
		return;
	}

	//Can't rest if you're trying to move
	Player->Player.SetRestingStatus(NOT_RESTING);

	NewRoom = RoomPtr->GetExit( WEST );

	if( !NewRoom )
	{
		WriteToBuffer( Player, "%sThere is no exit to the west!%s\n\r",
			ANSI_BR_RED, ANSI_WHITE);
		
		RoomPtr->SelectiveBroadcast( Player, NULL, "%s%s runs into the western wall!%s\n\r",
			ANSI_BR_YELLOW, Player->Player.GetFirstName(), ANSI_WHITE );
		
		return;
	}

	RoomPtr->RemovePlayerFromRoom( Player );
	RoomPtr->BroadcastRoom( "%s%s leaves to the west.%s\n\r", ANSI_BR_GREEN, Player->Player.GetFirstName(), ANSI_WHITE );
	NewRoom->BroadcastRoom( "%s%s walks in from the east%s\n\r", ANSI_BR_GREEN, Player->Player.GetFirstName(), ANSI_WHITE );
	NewRoom->AddPlayerToRoom( Player );
	Player->Player.SetRoom( NewRoom );
	WalkingLook( Player, NewRoom );
	
	return;
}


COMMAND(MoveNE)
{
	Room	*RoomPtr = '\0';
	Room	*NewRoom = '\0';

	RoomPtr = Player->Player.GetRoom();

	if( !RoomPtr )
	{
		WriteToBuffer( Player, "There was an error\n\r" );
		return;
	}

	//Can't rest if you're trying to move
	Player->Player.SetRestingStatus(NOT_RESTING);

	NewRoom = RoomPtr->GetExit( NORTHEAST );

	if( !NewRoom )
	{
		WriteToBuffer( Player, "%sThere is no exit to the northeast!%s\n\r",
			ANSI_BR_RED, ANSI_WHITE);
		
		RoomPtr->SelectiveBroadcast( Player, NULL, "%s%s runs into the northeastern wall!%s\n\r",
			ANSI_BR_YELLOW, Player->Player.GetFirstName(), ANSI_WHITE );
		
		return;
	}

	RoomPtr->RemovePlayerFromRoom( Player );
	RoomPtr->BroadcastRoom( "%s%s leaves to the northeast.%s\n\r", ANSI_BR_GREEN, Player->Player.GetFirstName(), ANSI_WHITE );
	NewRoom->BroadcastRoom( "%s%s walks in from the southwest%s\n\r", ANSI_BR_GREEN, Player->Player.GetFirstName(), ANSI_WHITE );
	NewRoom->AddPlayerToRoom( Player );
	Player->Player.SetRoom( NewRoom );
	WalkingLook( Player, NewRoom );
	
	return;
}

COMMAND(MoveSE)
{
	Room	*RoomPtr = '\0';
	Room	*NewRoom = '\0';

	RoomPtr = Player->Player.GetRoom();

	if( !RoomPtr )
	{
		WriteToBuffer( Player, "There was an error\n\r" );
		return;
	}

	//Can't rest if you're trying to move
	Player->Player.SetRestingStatus(NOT_RESTING);

	NewRoom = RoomPtr->GetExit( SOUTHEAST );

	if( !NewRoom )
	{
		WriteToBuffer( Player, "%sThere is no exit to the southeast!%s\n\r",
			ANSI_BR_RED, ANSI_WHITE);

		RoomPtr->SelectiveBroadcast( Player, NULL, "%s%s runs into the southeastern wall!%s\n\r",
			ANSI_BR_YELLOW, Player->Player.GetFirstName(), ANSI_WHITE );

		return;
	}

	RoomPtr->RemovePlayerFromRoom( Player );
	RoomPtr->BroadcastRoom( "%s%s leaves to the southeast.%s\n\r", ANSI_BR_GREEN, Player->Player.GetFirstName(), ANSI_WHITE );
	NewRoom->BroadcastRoom( "%s%s walks in from the northwest%s\n\r", ANSI_BR_GREEN, Player->Player.GetFirstName(), ANSI_WHITE );
	NewRoom->AddPlayerToRoom( Player );
	Player->Player.SetRoom( NewRoom );
	WalkingLook( Player, NewRoom );
	
	return;
}

COMMAND(MoveSW)
{
	Room	*RoomPtr = '\0';
	Room	*NewRoom = '\0';

	RoomPtr = Player->Player.GetRoom();

	if( !RoomPtr )
	{
		WriteToBuffer( Player, "There was an error\n\r" );
		return;
	}

	//Can't rest if you're trying to move
	Player->Player.SetRestingStatus(NOT_RESTING);

	NewRoom = RoomPtr->GetExit( SOUTHWEST );

	if( !NewRoom )
	{
		WriteToBuffer( Player, "%sThere is no exit to the southwest!%s\n\r",
			ANSI_BR_RED, ANSI_WHITE);

		RoomPtr->SelectiveBroadcast( Player, NULL, "%s%s runs into the southwestern wall!%s\n\r",
			ANSI_BR_YELLOW, Player->Player.GetFirstName(), ANSI_WHITE );

		return;
	}

	RoomPtr->RemovePlayerFromRoom( Player );
	RoomPtr->BroadcastRoom( "%s%s leaves to the southwest.%s\n\r", ANSI_BR_GREEN, Player->Player.GetFirstName(), ANSI_WHITE );
	NewRoom->BroadcastRoom( "%s%s walks in from the northeast%s\n\r", ANSI_BR_GREEN, Player->Player.GetFirstName(), ANSI_WHITE );
	NewRoom->AddPlayerToRoom( Player );
	Player->Player.SetRoom( NewRoom );
	WalkingLook( Player, NewRoom );
	
	return;
}

COMMAND(MoveNW)
{
	Room	*RoomPtr = '\0';
	Room	*NewRoom = '\0';

	RoomPtr = Player->Player.GetRoom();

	if( !RoomPtr )
	{
		WriteToBuffer( Player, "There was an error\n\r" );
		return;
	}

	//Can't rest if you're trying to move
	Player->Player.SetRestingStatus(NOT_RESTING);

	NewRoom = RoomPtr->GetExit( NORTHWEST );

	if( !NewRoom )
	{
		WriteToBuffer( Player, "%sThere is no exit to the northwest!%s\n\r",
			ANSI_BR_RED, ANSI_WHITE);

		RoomPtr->SelectiveBroadcast( Player, NULL, "%s%s runs into the northwestern wall!%s\n\r",
			ANSI_BR_YELLOW, Player->Player.GetFirstName(), ANSI_WHITE );

		return;
	}

	RoomPtr->RemovePlayerFromRoom( Player );
	RoomPtr->BroadcastRoom( "%s%s leaves to the northwest.%s\n\r", ANSI_BR_GREEN, Player->Player.GetFirstName(), ANSI_WHITE );
	NewRoom->BroadcastRoom( "%s%s walks in from the southeast%s\n\r", ANSI_BR_GREEN, Player->Player.GetFirstName(), ANSI_WHITE );
	NewRoom->AddPlayerToRoom( Player );
	Player->Player.SetRoom( NewRoom );
	WalkingLook( Player, NewRoom );
	
	return;
}



COMMAND(Teleport)
{
	Room			*Rom = '\0';
	Room			*CurRom = '\0';
	unsigned int	RoomNum = 0;
	
	RoomNum = atoi( Argument );

	Rom = SearchForRoom( RoomNum );

	if( !Rom )
	{
		WriteToBuffer( Player, "%s%d is not a valid room!%s\n\r",
			ANSI_BR_CYAN, RoomNum, ANSI_WHITE );
		return;
	}

	//Can't rest if you're teleporting
	Player->Player.SetRestingStatus(NOT_RESTING);

	CurRom = Player->Player.GetRoom();
	CurRom->RemovePlayerFromRoom( Player );
	CurRom->BroadcastRoom( "%s%s disappears in a flash of light!%s\n\r",
		ANSI_BR_RED, Player->Player.GetFirstName(), ANSI_WHITE );

	Rom->BroadcastRoom( "%s%s appears in a flash of light!%s\n\r",
		ANSI_BR_RED, Player->Player.GetFirstName(), ANSI_WHITE );
	Rom->AddPlayerToRoom( Player );
	Player->Player.SetRoom( Rom );
	
	WriteToBuffer( Player, "%s\t*** You have been transported to Room: %d. ***%s\n\r", 
		ANSI_BR_CYAN, RoomNum, ANSI_WHITE );
	
	WalkingLook( Player, Rom );	
	
	return;
}

COMMAND(Inventory)
{
	ItemsOnPlayer	*ListPtr = '\0';

	ListPtr = Player->Player.GetFirstItem();

	if( !ListPtr )
	{
		WriteToBuffer( Player, "%sYou have nothing in your inventory!%s\n\r",
			ANSI_BR_RED, ANSI_WHITE);
		return;
	}

	WriteToBuffer( Player, "%s\tYour Inventory\n\r=-=-=-=-=-=-=-=-=-=-=-=-=-=-=%s\n\r",
		ANSI_BR_CYAN, ANSI_WHITE);

	for( ; ListPtr; ListPtr = ListPtr->Next )
	{
		WriteToBuffer( Player, "%s", ListPtr->Item->GetItemName() );
		
		if( ListPtr->Item == Player->Player.GetWieldedItem() )
			WriteToBuffer( Player, " ( Weapon Hand )\n\r" ); //TODO: Fix this bug (multiple items wielded)
		else
			WriteToBuffer( Player, "\n\r" );
	}

	return;
}

COMMAND(GetItem)
{
	Item			*TempItem = '\0';
	Room			*TempRoom = '\0';
	ItemsInRoom		*ItemList = '\0';
	int				Len;

	if(!Player)return;
	if(!Argument)
	{
		WriteToBuffer( Player, "%sUsage:%s get <item>\n\r",
			ANSI_RED, ANSI_WHITE );
		return;
	}

	Len = strlen( Argument );	
	TempRoom = Player->Player.GetRoom();

	if( !TempRoom )
	{
		WriteToBuffer( Player, "Error in GetItem()\n\r" );
		return;
	}

	ItemList = TempRoom->GetFirstItem();

	if( !ItemList )
	{
		WriteToBuffer( Player, "%sYou dont see a %s here!%s\n\r",
			ANSI_BR_RED, Argument, ANSI_WHITE );
		return;
	}

	for( ; ItemList; ItemList = ItemList->Next )
	{
		if( !strnicmp( ItemList->Item->GetItemName(), Argument, Len ) )
		{
			TempItem = ItemList->Item;
			break;
		}
	}

	if( !TempItem )
	{
		WriteToBuffer( Player, "%sYou dont see a %s here!%s\n\r",
			ANSI_BR_RED, Argument, ANSI_WHITE );
	
		return;
	}


	
	if (Player->Player.AddItemToPlayer( TempItem ))
	{	
		TempRoom->RemoveItemFromRoom( TempItem );
		TempRoom->SelectiveBroadcast( Player, NULL, "%s%s picks up a %s%s\n\r", ANSI_BR_BLUE,
			Player->Player.GetFirstName(), TempItem->GetItemName(), ANSI_WHITE ); 
		WriteToBuffer( Player, "%sYou pick up a %s%s\n\r", ANSI_BR_BLUE,
			TempItem->GetItemName(), ANSI_WHITE );
	}

	return;
}


COMMAND(DropItem)
{
	Item			*TempItem = '\0';
	Room			*TempRoom = '\0';
	
	if( !Argument )
	{
		WriteToBuffer( Player, "%sUsage:%s drop <item>\n\r", ANSI_BR_RED,
			ANSI_WHITE );
		return;
	}

	TempRoom = Player->Player.GetRoom();

	if( !TempRoom )
	{
		WriteToBuffer( Player, "Error in DropItem()\n\r" );
		return;
	}

	TempItem = Player->Player.SearchPlayerForItem( Argument );

	if( !TempItem )
	{
		WriteToBuffer( Player, "%sYou dont have a %s to drop!%s\n\r",
			ANSI_BR_RED, Argument, ANSI_WHITE );
		return;
	}

	if (TempItem == Player->Player.IsWearing(TempItem->GetWearLocation()) )
	{
		WriteToBuffer( Player, "%sYou remove the %s!%s\n\r",
			ANSI_BR_BLUE, TempItem->GetItemName(), ANSI_WHITE );

		TempRoom->SelectiveBroadcast( Player, NULL, "%s%s removes %s!%s\n\r",
			ANSI_BR_BLUE, Player->Player.GetFirstName(), TempItem->GetItemName(), ANSI_WHITE );
	}

	//	if( TempItem == Player->Player.GetWieldedItem() )
//		Player->Player.WieldItem( '\0' );
	
	TempRoom->AddItemToRoom( TempItem );
	Player->Player.RemoveItemFromPlayer( TempItem );
	
	TempRoom->SelectiveBroadcast( Player, NULL, "%s%s drops a %s%s\n\r", ANSI_BR_BLUE,
		Player->Player.GetFirstName(), TempItem->GetItemName(), ANSI_WHITE ); 
	
	WriteToBuffer( Player, "%sYou drop the %s%s\n\r", ANSI_BR_BLUE,
		TempItem->GetItemName(), ANSI_WHITE );

	return;
}

COMMAND(GiveItem)
{
	Item			*TempItem = '\0';
	Room			*TempRoom = '\0';
	Connection		*Victim = '\0';
	
	if( !Argument || !Argument2 )
	{
		WriteToBuffer( Player, "%sUsage:%s give <item> <player>\n\r",
			ANSI_BR_RED, ANSI_WHITE );

		return;
	}
	
	int	Len = strlen( Argument );

	/*  Get the room we're dealing with  */
	TempRoom = Player->Player.GetRoom();

	if( !TempRoom )
	{
		WriteToBuffer( Player, "Error in CmdGiveItem()\n\r" );
		return;
	}

	/* have the room search itself for the victim */
	Victim = TempRoom->SearchRoomForPlayer( Argument2 );

	/*  if no target then bail */
	if( !Victim )
	{
		WriteToBuffer( Player, "%sYou don't see %s here!%s\n\r",
			ANSI_BR_RED, Argument2, ANSI_WHITE );

		return;
	}

	/*  have Client object search Itself for the item */
	TempItem = Player->Player.SearchPlayerForItem( Argument );

	if( !TempItem )
	{
		WriteToBuffer( Player, "%sYou dont have a %s to give!%s\n\r",
			ANSI_BR_RED, Argument, ANSI_WHITE );
		return;
	}

	/* if they are wielding the item then unwield it fer em */
	if( TempItem == Player->Player.GetWieldedItem() )
		Player->Player.WieldItem( '\0' );

	/* do the transfer of the item from Player to Victim */
	if ( Victim->Player.AddItemToPlayer( TempItem ) )
	{
		Player->Player.RemoveItemFromPlayer( TempItem );
	
	
		/* tell the room about it */
		TempRoom->SelectiveBroadcast( Player, Victim, "%s%s gives a %s to %s%s\n\r",
			ANSI_BR_BLUE, Player->Player.GetFirstName(), TempItem->GetItemName(),
			Victim->Player.GetFirstName(), ANSI_WHITE ); 
	
		/* tell the giver */
		WriteToBuffer( Player, "%sYou give the %s to %s%s\n\r",
			ANSI_BR_BLUE, TempItem->GetItemName(), Victim->Player.GetFirstName(), ANSI_WHITE );

		/*  tell the reciever  */
		WriteToBuffer( Victim, "%s%s gives you a %s%s\n\r",
			ANSI_BR_BLUE, Player->Player.GetFirstName(),
			TempItem->GetItemName(), ANSI_WHITE );
	}
	else
		WriteToBuffer( Player, "%s%s can't carry that much weight!%s\n\r",
			ANSI_BR_BLUE, Victim->Player.GetFirstName(), ANSI_WHITE );
	return;
}

COMMAND(Wield)
{
	Item			*TempItem = '\0';
	Item			*WieldedItem = '\0';
	Room			*TempRoom = '\0';

	if( !Argument )
	{
		WriteToBuffer( Player, "%sUsage:%s wield <item>\n\r",
			ANSI_BR_RED, ANSI_WHITE );
		return;
	}
	
	TempItem = Player->Player.SearchPlayerForItem( Argument );

	// Make sure it's a wieldable item!
	if ( TempItem->GetItemType() != ITEM_WEAPON)
	{
		WriteToBuffer( Player, "%sYou cannot wield a %s!%s\n\r",
			ANSI_BR_RED, Argument, ANSI_WHITE );
		return;
	}

	if( !TempItem )
	{
		WriteToBuffer( Player, "%sYou dont have a %s to wield!%s\n\r",
			ANSI_BR_RED, Argument, ANSI_WHITE );
		return;
	}

	TempRoom = Player->Player.GetRoom();

	/* If they allready have something wielded remove it */
	WieldedItem = Player->Player.GetWieldedItem();

	if( WieldedItem )
	{
		WriteToBuffer( Player, "%sYou remove your %s!%s\n\r",
			ANSI_BR_BLUE, WieldedItem->GetItemName(), ANSI_WHITE );

		TempRoom->SelectiveBroadcast( Player, NULL, "%s%s un-wields a %s!%s\n\r",
			ANSI_BR_BLUE, Player->Player.GetFirstName(), WieldedItem->GetItemName(),
			ANSI_WHITE );
	}

	WriteToBuffer( Player, "%sYou wield the %s!%s\n\r",
		ANSI_BR_BLUE, TempItem->GetItemName(), ANSI_WHITE );

	Player->Player.WieldItem( TempItem );
	
	TempRoom->SelectiveBroadcast( Player, NULL, "%s%s wields a %s!%s\n\r",
		ANSI_BR_BLUE, Player->Player.GetFirstName(), TempItem->GetItemName(),
		ANSI_WHITE );

	return;

}

/*===================================================================
COMMAND(Wear)

===================================================================*/
COMMAND(Wear)
{
	int             WearLocation = 0;
	Item			*TempItem = '\0';
	Item			*WornItem = '\0';
	Room			*TempRoom = '\0';

	if( !Argument )
	{
		WriteToBuffer( Player, "%sUsage:%s wear <item>\n\r",
			ANSI_BR_RED, ANSI_WHITE );
		return;
	}
	
	TempItem = Player->Player.SearchPlayerForItem( Argument );

	if( !TempItem )
	{
		WriteToBuffer( Player, "%sYou dont have a %s!%s\n\r",
			ANSI_BR_RED, Argument, ANSI_WHITE );
		return;
	}

	// Make sure it's a wearable item!
	if ( TempItem->GetItemType() != ITEM_WEAR)
	{
		WriteToBuffer( Player, "%sYou cannot wear a %s!%s\n\r",
			ANSI_BR_RED, Argument, ANSI_WHITE );
		return;
	}

	TempRoom = Player->Player.GetRoom();

	// Where is it worn?
	WearLocation = TempItem->GetWearLocation();

	// Is player wearing someting in new item's location already?
	if ( Player->Player.IsWearing(WearLocation) )
	{
		WornItem = Player->Player.IsWearing(WearLocation);
		Player->Player.AdjustPlayerStatsByItem( WornItem, REMOVE );
		WriteToBuffer( Player, "%sYou remove your %s!%s\n\r",
			ANSI_BR_BLUE, WornItem->GetItemName(), ANSI_WHITE );

		TempRoom->SelectiveBroadcast( Player, NULL, "%s%s removes a %s!%s\n\r",
			ANSI_BR_BLUE, Player->Player.GetFirstName(), WornItem->GetItemName(),
			ANSI_WHITE );
	}

	WriteToBuffer( Player, "%sYou put on the %s!%s\n\r",
		ANSI_BR_BLUE, TempItem->GetItemName(), ANSI_WHITE );
	Player->Player.WearItem( TempItem, WearLocation );
	Player->Player.AdjustPlayerStatsByItem( TempItem, ADD );
	
	TempRoom->SelectiveBroadcast( Player, NULL, "%s%s wears a %s!%s\n\r",
		ANSI_BR_BLUE, Player->Player.GetFirstName(), TempItem->GetItemName(),
		ANSI_WHITE );

	return;

}



//==================================================================
COMMAND(Remove)
{
	Item	*TempItem = '\0';
	Room	*TempRoom = '\0';
	int      ItemWearLocation;

	if( !Argument )
	{
		WriteToBuffer( Player, "%sUsage:%s remove <item>\n\r",
			ANSI_BR_RED, ANSI_WHITE );
		return;
	}
	
	// Does the player have this item on his/her person?
	TempItem = Player->Player.SearchPlayerForItem( Argument );
    // Wear is this items worn or wielded
	ItemWearLocation = TempItem->GetWearLocation();
    // Ensure the player has the item and he/she is wearing / wielding the item
	if( ! ( TempItem && Player->Player.IsWearing(ItemWearLocation) ) )
	{
		WriteToBuffer( Player, "%sYou cannot remove an item you aren't using!%s\n\r",
			ANSI_BR_RED, ANSI_WHITE );
		return;
	}


	TempRoom = Player->Player.GetRoom();

	WriteToBuffer( Player, "%sYou remove the %s!%s\n\r",
		ANSI_BR_BLUE, TempItem->GetItemName(), ANSI_WHITE );

	TempRoom->SelectiveBroadcast( Player, NULL, "%s%s removes %s!%s\n\r",
		ANSI_BR_BLUE, Player->Player.GetFirstName(), TempItem->GetItemName(),
		ANSI_WHITE );
	
	Player->Player.WearItem( NULL, ItemWearLocation );
	Player->Player.AdjustPlayerStatsByItem( TempItem, REMOVE );

}

/**********************************************************
 *  This is the look when ya dont wanna say               *
 *  this player has looked around the room                *
 **********************************************************/
void WalkingLook( Connection *Conn, Room *Rom )
{
	if ( !Conn )
		return;

	Item			*TempItem ='\0';
	ItemsInRoom		*ItemList = '\0';
				
	/* if passed a null room, then get the room from the Player */
	if( !Rom )
		Rom = Conn->Player.GetRoom();

	/* If that dont work then we're screwed, bail out */
	if( !Rom )
		return;
	
	/* ouput the room description */
	WriteToBuffer( Conn, "%s%s\n\r%s%s\n\r", ANSI_BR_GREEN, Rom->GetShortDesc(),
		ANSI_WHITE, Rom->GetLongDesc() );

	if( Rom->GetItemCount() > 0 )
	{
		ItemList = Rom->GetFirstItem();
		
		if( !ItemList )
		{
			WriteToBuffer( Conn, "Getting items on floor messed up!\n\r" );
			return;
		}
		
		WriteToBuffer( Conn, "%sYou notice: ", ANSI_BR_CYAN );
		
		for( int Count = 0; ItemList; ItemList = ItemList->Next )
		{
			if( Count == 0 )
				WriteToBuffer( Conn, "%s", ItemList->Item->GetItemName() );
			else
				WriteToBuffer( Conn, ", %s", ItemList->Item->GetItemName() );
			
			Count++;
		}
	
		WriteToBuffer( Conn, "%s\n\r", ANSI_WHITE );
	}

	if( Rom->GetPlayerCount() > 1 )
	WriteToBuffer( Conn, "%sAlso here: %s%s%s\n\r", ANSI_BR_GREEN, ANSI_BR_MAGENTA, Rom->GetAlsoHereString( Conn ), ANSI_WHITE );

	WriteToBuffer( Conn, "%sObvious exits: %s%s\n\r", ANSI_BR_YELLOW,
		Rom->GetExitString(), ANSI_WHITE );

	return;
}


/**********************************************************
 *  Emote  command                                        *
 **********************************************************/
COMMAND(Emote)
{
	char			*EmoteArg = '\0';
	Connection		*TempConn = '\0';
	Room			*TempRoom = '\0';

	TempRoom = Player->Player.GetRoom();
	
	if( !TempRoom )
		return;
	
	EmoteArg = FullCmd;

	while( *EmoteArg )
	{
		if( *EmoteArg == ' ' )
			break;

		EmoteArg++;
	}

	EmoteArg++;

	/* send it on out to all connections */
	TempRoom->BroadcastRoom( "%s%s %s%s\n\r", ANSI_BR_CYAN,
		Player->Player.GetFirstName(), EmoteArg, ANSI_WHITE );

	return;
}

COMMAND(Spawn)
{
	unsigned int	ItemNo = 0;
	Item			*TempItem = '\0';
	Room			*TempRoom = '\0';

	TempRoom = Player->Player.GetRoom();

	if( !Argument || !TempRoom )
	{
		WriteToBuffer( Player, "%sUsage:%s spawn <itemnumber>\n\r",
			ANSI_BR_RED, ANSI_WHITE );
		return;
	}

	ItemNo = atoi( Argument );

	TempItem = SearchForItem( ItemNo );

	if( !TempItem )
	{
		WriteToBuffer( Player, "%sSorry but there is no Item# %d.%s\n\r",
			ANSI_BR_RED, ItemNo, ANSI_WHITE );

		return;
	}

	/*  physically add it to thier inventory */
	if ( Player->Player.AddItemToPlayer( TempItem ) )
	{
		/* tell everyone who needs to know  */
		TempRoom->BroadcastRoom( "%s%s pulls a %s out of thin air!%s\n\r", ANSI_BR_CYAN,
			Player->Player.GetFirstName(), TempItem->GetItemName(), ANSI_WHITE );

		WriteToBuffer( Player, "%sA %s has been added to your inventory!%s\n\r",
			ANSI_BR_RED, TempItem->GetItemName(), ANSI_WHITE );
	}
	return;
}

/*=============================================================================
 Rest command
 ============================================================================*/

COMMAND(Rest)
{
	Room			*TempRoom = '\0';
    
	//if (Player->Player.
	// Get room so we can tell everyone in room
	TempRoom = Player->Player.GetRoom();
    // Player is now resting so tell the damn computer that!
	Player->Player.SetRestingStatus(RESTING);
	StopCombat(Player);
	// Lets tell the aholes in the room who is resting
	TempRoom->SelectiveBroadcast( Player, NULL, "%s%s stops to rest%s\n\r", ANSI_BR_BLUE,
	Player->Player.GetFirstName(), ANSI_WHITE ); 
	// I'm stupid tell me I'm resting.
	WriteToBuffer( Player, "%sYou stop to rest%s\n\r", ANSI_BR_BLUE, ANSI_WHITE );

	return;
}
