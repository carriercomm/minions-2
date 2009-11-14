/***************************************************************
 *           Minions Mud Server is copyright (C)2001           *
 *                    SonzoSoft Software                       *
 *                                                             *
 *         Programmed by Mark Richardson and David Brown       *
 *															   *
 *			 Mark Richardson	-	sinbaud@hotmail.com	       *
 *		       David Brown	    -	dcbrown73@yahoo.com	       *
 ***************************************************************/
#ifndef _ANSICOLOR_H_INCLUDED
#define _ANSICOLOR_H_INCLUDED


/*  cursor manipulation */
#define ANSI_CLR_SCR     			"\x1B[2J"	//clear screen and home cursor
#define ANSI_CLR_LINE				"\x1B[K"	//clear to end of line
#define ANSI_CLR_SOL				"\x1B[80;1f" //clear to start of line *dependent on the size of prompt

/* normal video display Ansi colors  */
#define ANSI_BLACK		"\x1B[0;30m"
#define ANSI_RED		"\x1B[0;31m"
#define ANSI_GREEN		"\x1B[0;32m"
#define ANSI_YELLOW		"\x1B[0;33m"
#define ANSI_BLUE		"\x1B[0;34m"
#define ANSI_MAGENTA	"\x1B[0;35m"
#define ANSI_CYAN		"\x1B[0;36m"
#define ANSI_WHITE		"\x1B[0;37m"

/* bold (lighter) shades of the ansi colors  */
#define ANSI_BR_RED			"\x1B[1;31m"
#define ANSI_BR_GREEN		"\x1B[1;32m"
#define ANSI_BR_YELLOW		"\x1B[1;33m"
#define ANSI_BR_BLUE		"\x1B[1;34m"
#define ANSI_BR_MAGENTA		"\x1B[1;35m"
#define ANSI_BR_CYAN		"\x1B[1;36m"
#define ANSI_BR_WHITE		"\x1B[1;37m"

/*  Underlined Ansi colors  */
#define ANSI_U_RED			"\x1B[4;31m"
#define ANSI_U_GREEN		"\x1B[4;32m"
#define ANSI_U_YELLOW		"\x1B[4;33m"
#define ANSI_U_BLUE			"\x1B[4;34m"
#define ANSI_U_MAGENTA		"\x1B[4;35m"
#define ANSI_U_CYAN			"\x1B[4;36m"
#define ANSI_U_WHITE		"\x1B[4;37m"

/*background and foreground color combos  */
#define ANSI_REDBG_YELLOWFG				"\x1B[1;41;33m"
//more to go just aint typed em in yet

#endif