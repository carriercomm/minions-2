/***************************************************************
 *           Minions Mud Server is copyright (C)2001           *
 *                    SonzoSoft Software                       *
 *                                                             *
 *         Programmed by Mark Richardson and David Brown       *
 *															   *
 *			 Mark Richardson	-	sinbaud@hotmail.com	       *
 *		       David Brown	    -	dbrown@centliq.com	       *
 ***************************************************************/
#if !defined ROOM_H_INCLUDED
#define ROOM_H_INCLUDED

#define MAX_SHORT_DESC			81   //one line of text
#define MAX_LONG_DESC			321 //should be 4 lines of text
#define MAX_PLAYERS_IN ROOM		20
#define MAX_EXITS				10
#define MAX_BROADCAST_SIZE		256
#define MAX_ALSO_HERE			450
#define MAX_EXIT_STRING			80



/**********************************************************
 *  Global variables                                      *
 **********************************************************/


/*  for easy refrence of exit directions */
enum ExitDirections
{
	NORTH, NORTHEAST, EAST, SOUTHEAST, SOUTH, SOUTHWEST, WEST,
	NORTHWEST, UP, DOWN
};

/*  Flag enumeration for Room Flags bitvector  */
enum
{
	RFLAG_EVIL, RFLAG_SAFEROOM
};


struct Connection; //forward declaration
class	Item;	   //forward declaration

/*  link list of players in this Room */
struct PlayersInRoom
{
	PlayersInRoom	*Next;
	Connection		*Player;
};

/*  link list of items on the floor in this room */
struct ItemsInRoom
{
	ItemsInRoom	*Next;
	Item		*Item;
};


/**********************************************************
 *  Room class definition                                 *
 **********************************************************/
class Room
{
	unsigned int	RoomNumber;
	unsigned int	ExitToRoomNumber[MAX_EXITS];
	int				PlayerCount;
	int				ItemCount;
	char			ShortDesc[MAX_SHORT_DESC];
	char			LongDesc[MAX_LONG_DESC];
	char			AlsoHereString[MAX_ALSO_HERE];
	PlayersInRoom	*FirstPlayer;
	ItemsInRoom		*FirstItem;
	Room			*Exit[MAX_EXITS];
	char			ExitString[MAX_EXIT_STRING];

public:
	unsigned long int	RoomFlags; //flags of the room
	Room();
	unsigned int GetRoomNumber( void ) { return RoomNumber; };
	void SetRoomNumber( unsigned int NewNumber ) { RoomNumber = NewNumber; };
	const char *GetShortDesc( void ) { return ShortDesc; };
	const char *GetLongDesc( void ) { return LongDesc; };
	bool SetShortDesc( char *NewDesc );
	bool SetLongDesc( char *NewDesc );
	void SetExit( Room *NewExit, int Direction ) { Exit[Direction] = NewExit; };
	Room *GetExit( int Direction ) { return Exit[Direction]; };
	void BroadcastRoom( char *format, ... );
	unsigned int GetExitToRoomNumber( unsigned int Dir ) { return ExitToRoomNumber[Dir]; };
	void SetExitToRoomNumber( unsigned int NewDir, unsigned int NewNumber ) { ExitToRoomNumber[NewDir] = NewNumber; };
	void SetExitString( void );
	char *GetExitString( void ) { return ExitString; };
	bool AddPlayerToRoom( Connection *Player );
	bool RemovePlayerFromRoom( Connection *Player );
	PlayersInRoom *GetFirstPlayer( void ) { return FirstPlayer; };
	char *GetAlsoHereString( Connection *Conn );
	void SetAlsoHereString( void );
	int GetPlayerCount( void ) { return PlayerCount; };
	ItemsInRoom *GetFirstItem( void ) { return FirstItem; };
	bool AddItemToRoom( Item *ItemToAdd );
	bool RemoveItemFromRoom( Item *ItemToRemove );
	int GetItemCount( void ) { return ItemCount; };
	Connection *SearchRoomForPlayer( char *Name );
	Item *SearchRoomForItem( char *Name );
	void SelectiveBroadcast( Connection *Actor, Connection *Victim, char *Format, ... );
	~Room();
};

/**********************************************************
 *  structures needed for room stuff                      *
 **********************************************************/
/*  Link list of all the rooms in the mud  */
struct RoomList
{
	RoomList	*Next;
	Room		*Value;
};




int LoadRoomDatabase( void );
void FixExits( void );
Room *SearchForRoom( unsigned int RoomNum );
void exiterr(int exitcode);

#endif // ROOM_H_INCLUDED