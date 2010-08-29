/***************************************************************
 *           Minions Mud Server is copyright (C)2001           *
 *                    SonzoSoft Software                       *
 *                                                             *
 *         Programmed by Mark Richardson and David Brown       *
 *															   *
 *			 Mark Richardson	-	sinbaud@hotmail.com	       *
 *		       David Brown	    -	dcbrown73@yahoo.com	       *
 ***************************************************************/
#if !defined(TCPCOMM_H_INCLUDED)
#define TCPCOMM_H_INLCUDED

#include <time.h>
#include "player.h"


#define MAX_INPUT_BUFFER		256
#define MAX_OUTPUT_BUFFER		4096
#define MAX_COMMAND				256
#define SERVER_PORT				23
#define HIGH_WATER_MARK			(MAX_OUTPUT_BUFFER * .75)
#define LOGOFF_MESSAGE			"\n\rLeaving Minions...Goodbye!\n\r"
#define TITLE_SCREEN_PATH		"ansiscr.txt"
#define HELP_SCREEN_PATH		"helpscreen.txt"
#define SERVER_LOG_PATH			"logs\\serverlog.txt"
#define SERVER_VERSION			"v.01 alpha"
#define SERVER_SHUTDOWN_MSG		"\x1B[1;4;5;31m\t\tAn administrator is shutting down the server.\n\r\t\t\t\tClosing link now.\x1B[0m\n\r"
#define SHUTDOWN_PASSWORD		"Secret"
#define SQL_HOST				"192.168.0.101"
#define SQL_USER				"minionadmin"
#define SQL_PASSWORD			"minadminman"
#define SQL_DATABASE			"minions"
#define MAX_LOG_STR				2048
#define MAX_TIME_STAMP			26

/* macros to manipulate the Flags using above defines */
#define FLAG_IS_SET(Flags, Bit)		( (Flags) & (Bit) )
#define SET_PFLAG(Flags, Bit)		( (Flags) |= (Bit) )
#define REMOVE_PFLAG(Flags, Bit)	( (Flags) &= ~(Bit) )

/* Player Flags Bit vectors 32bits worth */
#define FLAG_HIDDEN			(1 <<  0)
#define FLAG_FIGHTING		(1 <<  1)
#define FLAG_BLIND			(1 <<  2)
#define FLAG_SNEAKING		(1 <<  3)
#define FLAG_GOSSIPS		(1 <<  4)
#define FLAG_TELEPATHS		(1 <<  5)
#define FLAG_HIDING			(1 <<  6)
#define FLAG_STUN   		(1 <<  7)
#define Bit8		(1 <<  8)
#define Bit9		(1 <<  9)
#define Bit10		(1 << 10)
#define Bit11		(1 << 11)
#define Bit12		(1 << 12)
#define Bit13		(1 << 13)
#define Bit14		(1 << 14)
#define Bit15		(1 << 15)
#define Bit16		(1 << 16)
#define Bit17		(1 << 17)
#define Bit18		(1 << 18)
#define Bit19		(1 << 19)
#define Bit20		(1 << 20)
#define Bit21		(1 << 21)
#define Bit22		(1 << 22)
#define Bit23		(1 << 23)
#define Bit24		(1 << 24)
#define Bit25		(1 << 25)
#define Bit26		(1 << 26)
#define Bit27		(1 << 27)
#define Bit28		(1 << 28)
#define Bit29		(1 << 29)
#define Bit30		(1 << 30)
#define Bit31		(1 << 31)


/*  status of connected socket  */
enum ConnectionStatus
{
	STATUS_LOGIN, STATUS_GET_NAME, STATUS_NEW_PLAYER,
    STATUS_GET_LAST_NAME, STATUS_AUTHENTICATED,
	STATUS_CONFIRM_FIRST_NAME, STATUS_NEW_PASSWORD,
    STATUS_GET_PASSWORD, STATUS_CONFIRM_PASSWORD, STATUS_HANGUP,
	STATUS_CHOOSE_RACE, STATUS_CHOOSE_CLASS,STATUS_REROLL
};


struct Connection
{
	unsigned int		Descriptor;
	int					InTop;
	int					OutTop;
	int					Strikes;
	char				InBuffer[MAX_INPUT_BUFFER];
	char				OutBuffer[MAX_OUTPUT_BUFFER];
	char				CmdLine[MAX_COMMAND];
	char				PrevCommand[MAX_COMMAND];
	bool				CommandReady;
	ConnectionStatus	Status;
	Connection			*Next;
	Client				Player;
	time_t				LogOnTime;
	time_t				LastActivity;
	Connection			*Victim; //are you fighting someone?
	unsigned long int	Flags; //blindness, curse, etc bit flags
};

/**********************************************************
 *  Function definitions for tcpcomm module               *
 **********************************************************/
bool StartupWinsock( void );
bool AcceptConnection( void );
void Disconnect( Connection *Conn );
bool ReadFromSocket( Connection *Conn );
bool WriteToSocket( Connection *Conn );
bool ReadFromBuffer( Connection *Conn );
bool WriteToBuffer( Connection *Conn, char *format, ... );
void Broadcast( Connection *Conn, char *format, ... );
void ServerShutDown( bool term_code );
bool Logon( Connection *Conn, char *Cmd );
void LoadAnsiTitleScreen( void );
void LoadHelpScreen( void );
Connection *SearchForPlayer( char *name );
bool DisplayFile( Connection *Conn, char *Text );
void CheckReadSet( void );
void CheckWriteSet( void );
void ServerLog( char *format, ... );
char *LookupHostName(char *IpAddress);


#endif //TCPCOMM_H_INCLUDED