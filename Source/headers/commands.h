/***************************************************************
 *           Minions Mud Server is copyright (C)2001           *
 *                    SonzoSoft Software                       *
 *                                                             *
 *         Programmed by Mark Richardson and David Brown       *
 *															   *
 *			 Mark Richardson	-	sinbaud@hotmail.com	       *
 *		       David Brown	    -	dcbrown73@yahoo.com	       *
 ***************************************************************/
#ifndef COMMANDS_H_INCLUDED
#define COMMANDS_H_INCLUDED


#define MAX_COMMAND_LENGTH			256
#define COMMAND(name) void Cmd##name##( Connection *Player, char *FullCmd, char *Argument, char *Argument2 )

class Room; //forward declaration

/*   structure of the command table   */
struct Command_Table
{
	char		*Command;
	char		*Padded;
	void		( *FuncPtr ) (Connection *Player, char *FullCmd, char *Argument, char *Argument2 );
    bool        AdminOnly;
	
};


/*  Commands we have so far */
void ParseCommand( Connection *Player, char *CmdLine );
void Say( Connection *Conn, char *Text );
void WalkingLook( Connection *Conn, Room *Rom );

/*  Commands in the command table  */
COMMAND(Quit);
COMMAND(ShutDown);
COMMAND(Credits);
COMMAND(Who);
COMMAND(Help);
COMMAND(Attack);
COMMAND(ClrScr);
COMMAND(BreakCombat);
COMMAND(Status);
COMMAND(Telepath);
COMMAND(Look);
COMMAND(MoveNorth);
COMMAND(MoveSouth);
COMMAND(MoveUp);
COMMAND(MoveDown);
COMMAND(MoveEast);
COMMAND(MoveWest);
COMMAND(MoveNE);
COMMAND(MoveSE);
COMMAND(MoveSW);
COMMAND(MoveNW);
COMMAND(Teleport);
COMMAND(Gossip);
COMMAND(Inventory);
COMMAND(GetItem);
COMMAND(DropItem);
COMMAND(GiveItem);
COMMAND(Wield);
COMMAND(Remove);
COMMAND(Emote);
COMMAND(Spawn);
COMMAND(Score);

#endif //COMMANDS_H_INCLUDED