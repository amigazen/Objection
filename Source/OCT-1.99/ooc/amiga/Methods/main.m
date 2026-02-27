/*    USER.M    Test file for Amiga Specific Methods        April 90, bjw */

/*
 *    This program tests out the message port class.  It creates a 
 *    message port and then tries to name it.  If the first attemp to
 *    create a unique named port fails, we try a second on.
 *
 *    This program may be run solo, or two copies run at the same time.
 *    
 *    15-April-90    bjw    Initial coding.
 */


#include <apps/MsgPort.h>
@includes

 
extern BOOL 	_msgTrace;

char	buffer[ 96 ];

/* --------------------------------------------------------------------- */

void
main( argc, argv )
    int    argc;
	char  *argv[];
{
	id  	port;

	port = [MsgPort new];
	/*  Try to find a unique portname: */
	if( [port findSTR:"first"] == nil )
		[port findSTR:"second"];

	printf( "This port named <%s>\n", [port str] );

	printf( "Hit return to continue : " );
	fgets( buffer, 95, stdin );

	Delay( 50L * 2 );
	[port free];

	_msgTrace = 1;  	// Show off what happened.
}	/* main */

