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
#include <winsock2.h>
#include <string.h>
#include <stdio.h>
#include <fstream>
#include <time.h>
#include "tcpcomm.h"
#include "ansicolor.h"
#include "commands.h"
#include "room.h"
#include "item.h"
#include "races.h"

using namespace std;

/***************************************************************
 *     Global variables                                        *
 ***************************************************************/
unsigned int	ServerSocket;
Connection		*PlayerList = NULL;
fd_set			ReadSet,WriteSet, ExceptionSet;
char			TitleScreen[MAX_OUTPUT_BUFFER];
char			HelpScreen[MAX_OUTPUT_BUFFER];
ofstream		LogFile;
unsigned int	NumConnections = 0;



/*  External globals  */
extern time_t		CurrentTime, BootTime;
extern RaceTable    *MasterRaceTable;
extern ClassTable   *MasterClassTable;
extern RoomList		*FirstRoomInList;
extern ItemList		*FirstItemInList;
//extern MYSQL		SQLConnection;

/***************************************************************
 * Function: StartupWinsock()								   *
 * Modified: 11/9/2000										   *
 *															   *
 * Starts up winsock and sets up the listening ServerSocket    *
 ***************************************************************/
bool StartupWinsock( void )
{
	WSAData			WsaData;
	WORD			VersionRequested=MAKEWORD(2,2);
	int				ErrorFlag;
	SOCKADDR_IN		Server;
	int				opt=1; //option to set linger flag
	linger			ling;
	
	ErrorFlag = WSAStartup( VersionRequested, &WsaData );
	
	if( ErrorFlag != 0 )
	{
		ServerLog( "WsaStartup Failed with error code: %d", WSAGetLastError() );
		ServerShutDown( false );
	}

	ServerSocket = socket( AF_INET, SOCK_STREAM, 0 ); //global listening socket
	
	if( ServerSocket == INVALID_SOCKET )
	{
		ServerLog( "socket() failed with error code: %d", WSAGetLastError() );
		ServerShutDown( true );       
	}
	
	/* Set the SO_REUSEADDR option - may not need this */
	ErrorFlag = setsockopt( ServerSocket, SOL_SOCKET, SO_REUSEADDR,
		                (char *)&opt, sizeof(opt) );
	
	if( ErrorFlag == SOCKET_ERROR )
	{
		ServerLog( "setsockopt() failed with error code: %d", WSAGetLastError() );
		ServerShutDown( true );
	}

	Server.sin_family = AF_INET;			//setup the sockaddr_in
	Server.sin_addr.s_addr = 0;				//struct used for upcoming
	Server.sin_port=htons( SERVER_PORT );	//network calls (eg. listen)	

	/* bind the socket to an interface  */
	if( bind( ServerSocket, (LPSOCKADDR)&Server, sizeof( Server ) ) != 0 )
	{
		ServerLog( "Bind() failed with error code: %d", WSAGetLastError() );
		ServerShutDown( true );
	}

	/*  set the SO_LINGER option  */
	ling.l_onoff = ling.l_linger = 0;
	setsockopt( ServerSocket, SOL_SOCKET,SO_LINGER, (char *)&ling,
			   sizeof( struct linger ) );

	/*  Set the socket to non-blocking mode  */
	ioctlsocket( ServerSocket, FIONBIO, (u_long *)&opt );
	listen( ServerSocket, 5 );		//listen for connections

	FD_ZERO(&ReadSet);					//zero out the new fd_set
	FD_ZERO(&ExceptionSet);				//zero out exception set
	FD_ZERO(&WriteSet);					//zero out the WriteSet
	
	FD_SET(ServerSocket,&ReadSet);      //insert the listening socket
	FD_SET( ServerSocket, &ExceptionSet );
	
	return true;
}

char *LookupHostName( char *IpAddress )
{
	hostent			*RemoteHost;

	RemoteHost = gethostbyaddr( (char *)&IpAddress, 4, PF_INET );
	
	if( RemoteHost == NULL )
	{
		ServerLog("gethostbyaddr() failed, Error Code: [%d]", WSAGetLastError() );
		return "Unknown Host Name";
	}
	
	return RemoteHost->h_name;
}

bool AcceptConnection( void )
{
	Connection		*NewConn, *TempConn;
	SOCKADDR_IN		NewConnection;
	unsigned int	NewSocket;
	int				len;


	len = sizeof(sockaddr);
	
	NewSocket = accept( ServerSocket, (sockaddr *)&NewConnection, &len );
	
	if( NewSocket == INVALID_SOCKET )
	{
		ServerLog( "\nAcceptConnection() failed accepting new connection; Error Code: %d", WSAGetLastError() );
		return false;
	}

	NewConn = new Connection;
	
	if( !NewConn )
	{
		ServerLog( "AcceptConnection(): Could not allocate a new Connection" );
		send( NewSocket, "No more memory left, try later!\n\r", 35, 0 );
		shutdown( NewSocket,1 );
		closesocket( NewSocket );
		return false;
	}

	FD_SET( NewSocket, &ReadSet );
	FD_SET( NewSocket, &WriteSet );
	FD_SET( NewSocket, &ExceptionSet );

	NewConn->Next = NULL;
	NewConn->OutTop = 0;
	NewConn->InTop = 0;
	NewConn->CmdLine[0] = '\0';
	NewConn->InBuffer[0] = '\0';
	NewConn->OutBuffer[0] = '\0';
	NewConn->Descriptor = NewSocket;
	NewConn->Status = STATUS_LOGIN;
	NewConn->Strikes = 0;
	time( &NewConn->LogOnTime );
	time( &NewConn->LastActivity );
	NewConn->Victim = '\0';
	NewConn->Flags = 0; //clear flags
	SET_PFLAG( NewConn->Flags, FLAG_GOSSIPS );
	SET_PFLAG( NewConn->Flags, FLAG_TELEPATHS );
	NewConn->CommandReady = true;

	/*  Get the IP and hostname of remote connection attempt */
	if( !NewConn->Player.SetIpAddress( inet_ntoa( NewConnection.sin_addr ) ) )
		NewConn->Player.SetIpAddress( "Unknown" );

	NewConn->Player.SetHostName( inet_ntoa( NewConnection.sin_addr ) );

	/* Log the connection */
	ServerLog( "%sAccepting connection request from [%s] - %s\n",
		ctime( &CurrentTime ), NewConn->Player.GetIpAddress(), NewConn->Player.GetHostName( ) );

	cout<<"Connection accepted from "<<NewConn->Player.GetIpAddress()<<" - "<<NewConn->Player.GetHostName()<<endl;
	
	if( PlayerList )
	{
		TempConn = PlayerList;
		PlayerList = NewConn;
		NewConn->Next = TempConn;
	}
	
	PlayerList = NewConn;
	
	DisplayFile( NewConn, TitleScreen );
	WriteToBuffer( NewConn, "\n\r\n\r%s%s\rActive connections: %d\n\r", ANSI_BR_GREEN, ctime( &CurrentTime ), NumConnections ); 
	WriteToBuffer( NewConn, "\n\r%sLogin or type 'new': %s",
		           ANSI_BR_YELLOW, ANSI_BR_WHITE);
	
	NumConnections++;

	return true;
}

bool ReadFromSocket( Connection *Conn )
{
	int Error = 0;
	int Start = 0;

    /* Hold horses if pending command already. */
    if ( Conn->CmdLine[0] != '\0' )
		return true;

    /* no overflows please. */
    Start = strlen( Conn->InBuffer );
    
	if ( Start >= MAX_INPUT_BUFFER - 10 )
    {
		send( Conn->Descriptor,"\n\rFor spamming you are kicked!\n\r", 33,0 );
		return false;
    }

    for ( ; ; )
    {
		int BytesRead;

		BytesRead = recv( Conn->Descriptor, Conn->InBuffer + Start, sizeof( Conn->InBuffer ) - 10 - Start, 0 );

		Error = WSAGetLastError ();

		if ( BytesRead > 0 )
		{
			Start += BytesRead;
			if ( Conn->InBuffer[Start-1] == '\n' || Conn->InBuffer[Start-1] == '\r' )
				break;
		}
		
		else if ( BytesRead == 0 )
		{
			return false;
		}
		
		else if ( Error == WSAEWOULDBLOCK )
			break;
	
		else
		{
			cout<<"ReadFromSocket() fucked off"<<endl;
			return false;
		}
	}

    Conn->InBuffer[Start] = '\0';
    return true;
}

bool WriteToSocket( Connection *Conn )
{
	int Start;
    int BytesWritten;
    int BlockSize;
	int Length;

	if( Conn->OutTop < 1 )
		return true;

    Length = strlen( Conn->OutBuffer );

    for ( Start = 0; Start < Length; Start += BytesWritten )
    {
		BlockSize = (Length - Start) < 4096 ? (Length - Start) : 4096;
		
		if ( ( BytesWritten = send( Conn->Descriptor, Conn->OutBuffer + Start, BlockSize, 0 ) ) < 0 )
	    { 
			if( WSAGetLastError() == WSAEWOULDBLOCK )
			{
				memmove( Conn->OutBuffer, Conn->OutBuffer + Start, Length - Start );
				Conn->OutTop = 0;
				ServerLog("had to call memmomove in WriteToSocket()\n");
				return true;
			}
			cout<<"WriteToSocket failed with eror code: "<<WSAGetLastError()<<endl;
			return false;
		}
    }
	
	Conn->OutBuffer[0] = '\0';
	Conn->OutTop = 0;
	
	return true;
}

bool ReadFromBuffer( Connection *Conn )
{
	int i;
	//int j;
	int	k;

    /*  Hold horses if pending command already. */
    if ( Conn->CmdLine[0] != '\0' )
		return true;

	/*  look for at least one new line */
    for ( i = 0; Conn->InBuffer[i] != '\n' && Conn->InBuffer[i] != '\r' && i < MAX_INPUT_BUFFER; i++ )
    {
		if ( Conn->InBuffer[i] == '\0' )
			return true;
	}

    /* input processing. */
    for ( i = 0, k = 0; Conn->InBuffer[i] != '\n' && Conn->InBuffer[i] != '\r'; i++ )
    {
		if ( k >= 254 )
		{
			WriteToBuffer( Conn, "Line too long.\n\r" );
				
			Conn->InBuffer[i] = '\n';
			Conn->InBuffer[i+1] = '\0';
			break;
		}

		if ( Conn->InBuffer[i] == '\b' && k > 0 )
			--k;
			
		else if ( isascii( Conn->InBuffer[i] ) && isprint( Conn->InBuffer[i] ) )
			Conn->CmdLine[k++] = Conn->InBuffer[i];

	}

	/* Finish off the line. */
    if ( k == 0 )
	Conn->CmdLine[k++] = ' ';
    Conn->CmdLine[k] = '\0';

    /* Do repeat command if they entered ','  */
    if ( Conn->CmdLine[0] == ',' )
	strcpy( Conn->CmdLine, Conn->PrevCommand );
    else
	strcpy( Conn->PrevCommand, Conn->CmdLine );

    /* Shift the input buffer.
     
    while ( Conn->InBuffer[i] == '\n' || Conn->InBuffer[i] == '\r' )
		i++;

    for ( j = 0; ( Conn->InBuffer[j] = Conn->InBuffer[i+j] ) != '\0'; j++ );
	*/
	Conn->InBuffer[0] = '\0';
    return true;
}


bool WriteToBuffer( Connection *Conn, char *format, ... )
{
	va_list		argument_list;
	char		TempString[512];
	int			Length;


	va_start( argument_list, format );
	vsprintf( TempString, format, argument_list );

	Length = strlen( TempString );

	/*if ( Conn->OutTop == 0 && Conn->CommandReady )
    {
		Conn->OutBuffer[0] = '\n';
		Conn->OutBuffer[1] = '\r';
		Conn->OutTop = 2;
    }*/

    /* No overflows please */
    if( Conn->OutTop + Length >= MAX_OUTPUT_BUFFER - 5 )
    {
        memset( Conn->OutBuffer, '\0', MAX_OUTPUT_BUFFER );
		Conn->OutTop = 0;
		return false;
	}

    /* Copy the shit into the damn buffer */
    strncpy( Conn->OutBuffer + Conn->OutTop, TempString, Length );
    Conn->OutTop += Length;
    Conn->OutBuffer[Conn->OutTop] = '\0';
    
	return true;
}

bool DisplayFile( Connection *Conn, char *Text )
{
	int			Length;

	Length = strlen( Text );

	if ( Conn->OutTop == 0 && !Conn->CommandReady )
    {
		Conn->OutBuffer[0] = '\n';
		Conn->OutBuffer[1] = '\r';
		Conn->OutTop = 2;
    }

    /* No overflows please */
    if( Conn->OutTop + Length >= MAX_OUTPUT_BUFFER - 5 )
    {
        memset( Conn->OutBuffer, '\0', MAX_OUTPUT_BUFFER );
		Conn->OutTop = 0;
		return false;
	}

    /* Copy the shit into the damn buffer */
    strncpy( Conn->OutBuffer + Conn->OutTop, Text, Length );
    Conn->OutTop += Length;
    Conn->OutBuffer[Conn->OutTop] = '\0';
    
	return true;
}

void Broadcast( Connection *Conn, char *format, ... )
{
	Connection		*TempConn;
	char			TempString[MAX_COMMAND];
	va_list			ArgumentList;

	va_start( ArgumentList, format );
	vsprintf( TempString, format, ArgumentList );
		
	for( TempConn = PlayerList; TempConn; TempConn = TempConn->Next )
	{
		if( TempConn->Status == STATUS_AUTHENTICATED )
		WriteToBuffer( TempConn, TempString );
	}

	return;
}

void Disconnect( Connection *Conn )
{
	Connection	*TempConn = '\0';
	Room		*TempRoom = '\0';

	send( Conn->Descriptor, LOGOFF_MESSAGE, strlen(LOGOFF_MESSAGE), 0 );

	/*   shutdown and close the socket connection  */
	shutdown( Conn->Descriptor ,SD_SEND );
	closesocket( Conn->Descriptor );

	/*  Announce their departure  */
	if( Conn->Status == STATUS_AUTHENTICATED )
	{
		Broadcast( Conn, "%s***[ %s left the realm. ]***%s\n\r", ANSI_BR_YELLOW,
			       Conn->Player.GetFirstName(), ANSI_BR_WHITE );
		ServerLog( "%s [%s] disconnects.\n", Conn->Player.GetFirstName(),
			       Conn->Player.GetIpAddress() );
	}

	else
		ServerLog( "[%s] disconnected without authenticating.\n", Conn->Player.GetIpAddress() );

	/*   Clear the socket from active descriptor sets   */
	FD_CLR( Conn->Descriptor, &ReadSet );
	FD_CLR( Conn->Descriptor, &WriteSet );
	FD_CLR( Conn->Descriptor, &ExceptionSet );
	
	/* Save thier character if they are logged in and remove them from room */
	if( Conn->Status == STATUS_AUTHENTICATED )
	{
		TempRoom = Conn->Player.GetRoom();
		Conn->Player.SetCurrentRoomNumber( TempRoom->GetRoomNumber() );
		TempRoom->RemovePlayerFromRoom( Conn );
		Conn->Player.SavePlayer();
	}

	/*  Hangup on them an free the resources they were hogging */
	if( PlayerList == Conn )
		PlayerList = Conn->Next;

	else
	{
		for( TempConn = PlayerList; TempConn; TempConn = TempConn->Next )
		{
			if( TempConn->Next == Conn )
				break;
		}

		TempConn->Next = Conn->Next;
	}

	delete Conn;

	NumConnections--;

	return;	
}


Connection *SearchForPlayer( char *name )
{
	Connection *cur;
	char *match;

	if( name == NULL )
		return NULL;
	
	*name = toupper( *name );

	for( cur = PlayerList; cur != NULL; cur = cur->Next )
	{
		match = strstr( cur->Player.GetFirstName(), name );
		if( match != NULL )
			return cur;
	}
	
	return NULL;
}

/***************************************************************
 * Function: Logon()                                           *
 * Modified: 11/10/2000										   *
 *															   *
 * Deals with Connections who have not been authenticated      *
 ***************************************************************/
bool Logon( Connection *Conn, char *Cmd )
{
	Connection		*Dummy = '\0';
	Room			*TempRom = '\0';
	RaceTable       *TempPtr = '\0';
	ClassTable      *TempCPtr = '\0';
	int             Choice = 0;

	if( Conn->Strikes > 1 )
	{
		send( Conn->Descriptor, "\n\rCome back when ya know a password\n\r", 36, 0);
		return false;
	}

	switch( Conn->Status )
	{
	case STATUS_LOGIN:
		
		if( !stricmp( Cmd, "new" ) )
		{
			WriteToBuffer( Conn, "\n\rWhat shall thy name be? " );
			Conn->Status = STATUS_GET_NAME;
			return true;
		}
		
		Dummy = SearchForPlayer( Cmd );
		if( Dummy )
		{
			WriteToBuffer( Conn, "\n\rThat account is already logged in.\n\rLogin or type 'new': " );
			Conn->Strikes++;
			return true;
		}

		if( !Conn->Player.LoadPlayer( Cmd ) )
		{
			WriteToBuffer( Conn, "\n\rThat account does not exist.\n\rLogin or type 'new': " );
			Conn->Strikes++;
			return true;
		}

		else
			WriteToBuffer( Conn, "\n\rPassword: " );
		
		Conn->Status = STATUS_GET_PASSWORD;
		break;

	case STATUS_GET_NAME:
		
		if( Conn->Player.LoadPlayer( Cmd ) )
		{
			WriteToBuffer( Conn, "\n\rThat name is allready taken.\n\rChoose another name: " );
			Conn->Status = STATUS_LOGIN;
			Conn->Strikes++;
			return true;
		}

		if( !Conn->Player.SetFirstName( Cmd ) )
		{
			WriteToBuffer( Conn, "\n\rThat name will not work, pick another: ");
			Conn->Strikes++;
			break;
		}
		
		WriteToBuffer( Conn, "\n\rAre you sure that %s is the name you want? (y/n): ", Conn->Player.GetFirstName() );
		Conn->Status = STATUS_CONFIRM_FIRST_NAME;
		break;

	case STATUS_CONFIRM_FIRST_NAME:
		
		if( !stricmp( Cmd, "y" ) )
		{
			WriteToBuffer( Conn, "\n\rVery well then, enter your surname: ");
			Conn->Status = STATUS_GET_LAST_NAME;
			break;
		}
		
		else 
			WriteToBuffer( Conn, "\n\rOk...Give me another damn name then: ");
		Conn->Strikes++;
		Conn->Status = STATUS_GET_NAME;
		break;
	
		
	case STATUS_GET_LAST_NAME:
		
		if( !Conn->Player.SetLastName( Cmd ) )
		{
			WriteToBuffer( Conn, "\n\rThat name will not work, type another: ");
			return true;
		}
		WriteToBuffer( Conn, "\n\rPassword: " );
		Conn->Status = STATUS_NEW_PASSWORD;
		break;

	case STATUS_NEW_PASSWORD:
		
		if( !Conn->Player.SetPassword( Cmd ) )
		{
			WriteToBuffer( Conn, "\n\rThat password will not work.\n\rPassword: " );
			Conn->Strikes++;
			return true;
		}

		else
			WriteToBuffer( Conn, "\n\rConfirm password: " );
		Conn->Status = STATUS_CONFIRM_PASSWORD;

		break;

	case STATUS_GET_PASSWORD:

		if( strcmp( Conn->Player.GetPassword(), Cmd ) )
		{
			WriteToBuffer( Conn, "\n\rIncorrect Password.\n\rEnter Password: " );
			Conn->Strikes++;
			return true;
		}
		else
			WriteToBuffer( Conn, "\n\rWelcome to Minions!\n\r" );
			WriteToBuffer( Conn, "Type 'help' for a list of commands.\n\r" );
			Broadcast( Conn, "%s***[ %s enters the realm. ]***%s\n\r", ANSI_BR_YELLOW,
				Conn->Player.GetFirstName(), ANSI_BR_WHITE  );
			Conn->Status = STATUS_AUTHENTICATED;
			Conn->Player.SetRoom( SearchForRoom( Conn->Player.GetCurrentRoomNumber() ) );
			TempRom = Conn->Player.GetRoom();
			TempRom->AddPlayerToRoom( Conn );
		break;

	case STATUS_CONFIRM_PASSWORD:

		if( strcmp( Cmd, Conn->Player.GetPassword() ) )
		{
			WriteToBuffer( Conn, "\n\rPasswords did not match.\n\rPassword: " );
			Conn->Strikes++;
			Conn->Status = STATUS_CONFIRM_PASSWORD;
			return true;
		}

		else
		{
			Conn->Status = STATUS_CHOOSE_RACE;
			
			WriteToBuffer( Conn, "\n\r%sRace Selection\n\r+------------+\n\r%s",
				ANSI_BR_YELLOW, ANSI_BR_GREEN );
			for( TempPtr = MasterRaceTable; TempPtr; TempPtr = TempPtr->Next )
			{
				WriteToBuffer( Conn, "[%i] - %s\n\r", TempPtr->RaceNumber,
				TempPtr->RaceName );
			}
			
			WriteToBuffer( Conn, "%sChoose a race: %s", ANSI_BR_WHITE,
				ANSI_WHITE );
		}

		break;

	case STATUS_CHOOSE_RACE:

		Choice = atoi( Cmd );

		for( TempPtr = MasterRaceTable; TempPtr; TempPtr = TempPtr->Next )
		{
			if( Choice == TempPtr->RaceNumber )
			{
				WriteToBuffer( Conn, "Ok, you have chosen to be %s\n\r", 
					TempPtr->RaceName );
				Conn->Player.SetRaceStr( TempPtr->RaceName );
				Conn->Player.SetRace( TempPtr->RaceNumber );
				Conn->Status = STATUS_CHOOSE_CLASS;
				WriteToBuffer( Conn, "%s%sClass Selection\n\r+-------------+%s\n\r",
					ANSI_CLR_SCR, ANSI_BR_YELLOW, ANSI_BR_GREEN );

				for( TempCPtr = MasterClassTable; TempCPtr; TempCPtr = TempCPtr->Next )
				{
					WriteToBuffer( Conn, "[%i] - %s\n\r", TempCPtr->ClassNumber,
						TempCPtr->ClassName );
				}

				WriteToBuffer( Conn, "%sChoose Your Class:%s ", 
					ANSI_BR_WHITE, ANSI_WHITE );

				return true;
			}
		}

		WriteToBuffer( Conn, "%s%sYou must choose a valid number!%s\n\r",
			ANSI_CLR_SCR, ANSI_BR_RED, ANSI_BR_GREEN );

		for( TempPtr = MasterRaceTable; TempPtr; TempPtr = TempPtr->Next )
		{
			WriteToBuffer( Conn, "[%i] - %s\n\r", TempPtr->RaceNumber,
				TempPtr->RaceName );
		}

		WriteToBuffer( Conn, "%sChoose a race:%s ", ANSI_BR_WHITE,
			ANSI_WHITE );
		break;
	
	case STATUS_CHOOSE_CLASS:
			
		Choice = atoi( Cmd );

		for( TempCPtr = MasterClassTable; TempCPtr; TempCPtr = TempCPtr->Next )
		{
			if( Choice == TempCPtr->ClassNumber )
			{
				WriteToBuffer( Conn, "Ok, you have chosen to be a %s\n\r", 
					TempCPtr->ClassName );
				Conn->Player.SetClassStr( TempCPtr->ClassName );
				Conn->Player.SetClass( TempCPtr->ClassNumber );
				
				WriteToBuffer( Conn, "%s%sWelcome to Minions!\n\r",
					ANSI_CLR_SCR, ANSI_BR_RED );
				WriteToBuffer( Conn, "Type 'help' for a list of commands.%s\n\r",
					ANSI_WHITE );
			    Broadcast( Conn, "%s***[ %s enters the realm. ]***%s\n\r", ANSI_BR_YELLOW,
				      Conn->Player.GetFirstName(), ANSI_BR_WHITE  );
			    Conn->Status = STATUS_AUTHENTICATED;
			    TempRom = SearchForRoom( 1 );
				Conn->Player.SetRoom( TempRom );
			    TempRom = Conn->Player.GetRoom();
			    TempRom->AddPlayerToRoom( Conn );
				return true;
			}
		}

		WriteToBuffer( Conn, "%s%sYou must choose a valid number!%s\n\r",
			ANSI_CLR_SCR, ANSI_BR_RED, ANSI_BR_GREEN );

		for( TempCPtr = MasterClassTable; TempCPtr; TempCPtr = TempCPtr->Next )
		{
			WriteToBuffer( Conn, "[%i] - %s\n\r", TempCPtr->ClassNumber,
				TempCPtr->ClassName );
		}

		WriteToBuffer( Conn, "%sChoose a class:%s ", ANSI_BR_WHITE,
			ANSI_WHITE );
		break;

	default:
		ServerLog( "Hit Default in Login() function!!!" );
		break;

	}
	return true;
}



/*
  ================================================================
   This loads the ansi title screen heh just like it fucking says
  ================================================================
*/

void LoadAnsiTitleScreen( void )
{
	ifstream	file;
	int			x=0;

	file.open(TITLE_SCREEN_PATH,ios::in|ios::binary);
	
	if(!file)
	{
		ServerLog( "Failed to open the Ansi title screen." );
		return;
	}

	while( !file.eof() )
	{
		file.get( TitleScreen[x] );
		x++;
	}
	
	file.close();
	return;
}

/*
  ================================================================
   This loads the ansi help screen heh just like it fucking says
  ================================================================
*/

void LoadHelpScreen( void )
{
	ifstream	file;
	int			x=0;

	file.open(HELP_SCREEN_PATH,ios::in|ios::binary);
	
	if(!file)
	{
		ServerLog( "Failed to open the help screen." );
		return;
	}

	while( !file.eof() )
	{
		file.get( HelpScreen[x] );
		x++;
	}
	
	file.close();
	return;
}
/***************************************************************
 *  CheckReadSet                                               *
 ***************************************************************/
void CheckReadSet( void )
{
	timeval		Timeout;
	fd_set		TempSet;
	int			Error;
	char		cmdline[MAX_COMMAND];
	Connection  *Conn = NULL, *Conn_Next = NULL;

	Timeout.tv_sec = 0L;
	Timeout.tv_usec = 75000L;
	TempSet = ReadSet;

	Error = select( 0, &TempSet, 0, 0, &Timeout );

	if( Error == SOCKET_ERROR )
	{
		ServerLog("select() in CheckReadSet() screwed up Error code: %d", WSAGetLastError() );
		ServerShutDown( true );
	}

	if( Error == 0 )
		return;

	if( FD_ISSET( ServerSocket, &TempSet ) )
	{
		AcceptConnection();
	}

	for ( Conn = PlayerList; Conn; Conn = Conn_Next )
	{
		Conn_Next = Conn->Next;   
			
		//Conn->CommandReady = false;
				
		if ( FD_ISSET( Conn->Descriptor, &TempSet ) )
		{
			if( !ReadFromSocket( Conn ) )
			{
				Conn->OutTop = 0;
				Disconnect( Conn );
				continue;
			}
			/*  started typing but no complete command yet */
			Conn->CommandReady = false;
			Conn->LastActivity = CurrentTime;
		}

		ReadFromBuffer( Conn );
		
		if ( Conn->CmdLine[0] != '\0' )
		{
			Conn->CommandReady = true;
			strcpy( cmdline, Conn->CmdLine );
			Conn->CmdLine[0] = '\0';
					
			switch( Conn->Status )
			{
				case STATUS_AUTHENTICATED:
					ParseCommand( Conn, cmdline );
					break;
				default:
					if( !Logon( Conn, cmdline ) )
					Disconnect( Conn );
					break;
			
			}
			
		}
	}
	return;
}

/***************************************************************
 *  CheckWriteSet											   *
 ***************************************************************/
void CheckWriteSet( void )
{
	timeval		Timeout;
	fd_set		TempSet, TempExcpSet;
	int			Error;
	Connection  *Conn = NULL;
	Connection  *Conn_Next = NULL;

	TempSet = WriteSet;
	TempExcpSet = ExceptionSet;
	Timeout.tv_sec = 0L;
	Timeout.tv_usec = 75000L;
	

	Error = select( 0, 0, &TempSet, &TempExcpSet, &Timeout );

	if( Error == 0 )
		return;

	if( Error == SOCKET_ERROR ) 
	{
		ServerLog( "select() in CheckWriteSet failed: %d", WSAGetLastError() );
		return;
	}

	/*  Output.  */
	for ( Conn = PlayerList; Conn; Conn = Conn_Next )
	{
		Conn_Next = Conn->Next;
	
		if ( ( Conn->CommandReady == true && Conn->OutTop > 0 ) && FD_ISSET( Conn->Descriptor, &TempSet) )
		{
			 if( !WriteToSocket( Conn ) )
			 {
				 Conn->OutTop = 0;
				 Disconnect( Conn );
			 }
			 
		}
		
		if( FD_ISSET( ServerSocket, &TempExcpSet ) )
		ServerLog( "Uh-Oh..Exception was raised on the listening socket!");
		
		if ( FD_ISSET( Conn->Descriptor, &TempExcpSet) )
		{
			 Conn->OutTop = 0;
			 Disconnect( Conn );
		}

	}
	return;
}

/***************************************************************
 *  Function: ServerLog()                                      *
 *  Modified: 7/1/2001                                         *
 *                                                             *
 *  Formats and then writes a string to the server's logfile   *
 ***************************************************************/
void ServerLog( char *format, ... )
{
	char		LogString[MAX_COMMAND];
	va_list		argument_list;
	
	va_start( argument_list, format );
	vsprintf( LogString, format, argument_list );

	LogFile<<LogString<<endl;
}

/***************************************************************
 * Function: ServerShutDown()                                  *
 * Modified: 11/9/2000										   *
 *															   *
 * Kills the server process if we have an unrecoverable error  *
 * and cleans up winsock.									   *
 ***************************************************************/
void ServerShutDown( bool term_code )
{
	Connection	*TempConn, *Next_Conn;
	RoomList	*TempRom, *Next_Rom;
	ItemList	*TempItem, *Next_Item;
	Room		*TempRoom = '\0';

	if(!term_code)
		exit(0);
	
	/*  release all the player memory used in the link list */
	if( PlayerList )
	{
		for( TempConn = PlayerList; TempConn; TempConn = Next_Conn )
		{
			Next_Conn = TempConn->Next;
			
			/* Send em a logoff message */
			send( TempConn->Descriptor, SERVER_SHUTDOWN_MSG, strlen( SERVER_SHUTDOWN_MSG ), 0 );
			cout<<"Saving and disconnecting: "<<TempConn->Player.GetFirstName()<<endl;
			closesocket( TempConn->Descriptor );
					
			/*  Save them if they are authenticated  */
			if( TempConn->Status == STATUS_AUTHENTICATED )
			{
				TempRoom = TempConn->Player.GetRoom();
				TempConn->Player.SetCurrentRoomNumber( TempRoom->GetRoomNumber() );
				TempConn->Player.SavePlayer();
			}
			
			delete TempConn;
		}

	}

	/*  release all the room memory used in the link list */
	if( FirstRoomInList )
	{
		int Count = 0;

		for( TempRom = FirstRoomInList; TempRom; TempRom = Next_Rom )
		{
			Next_Rom = TempRom->Next;

			delete TempRom->Value;
			delete TempRom;
			Count++;
		}

		cout<<"deleted "<<Count<<" Rooms."<<endl;

	}	

	/*  release all the items memory used in the link list */
	if( FirstItemInList )
	{
		int Count = 0;

		for( TempItem = FirstItemInList; TempItem; TempItem = Next_Item )
		{
			Next_Item = TempItem->Next;

			delete TempItem->Value;
			delete TempItem;
			Count++;
		}

		cout<<"deleted "<<Count<<" Items."<<endl;

	}	
	
	/*  release winsock subsystem */
	closesocket( ServerSocket );
	cout<<"Unloading winsock2.dll"<<endl;
	WSACleanup();

	/* close the LogFile handle  */
	cout<<"Closing Server's Log file"<<endl;
	LogFile.close();
	
	return;
}