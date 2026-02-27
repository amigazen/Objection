//    speed.m     Bild a speed test..                           Nov 91, bjw
//	:ts=8

//	This program is a speed test.  It measures the time to perform so
//	many (a changable factor).  It uses four class that inherit from
//	each other which inherits from class Object.  A minimal count of
//	different methods is used here to keep the cache as empty as
//	possible.
//
//	Once done, the entire cache is printed out.  On slow computers,
//	this program may take a while.  Uses time() to gather system time.
//	UNIX boxes (and other systems that support time zones) probably
//	need to call a different function for expanding the 'time_t'
//	structure.  AmigaDOS uses localtime().  Some systems may not
//	return hunderdths of a second :-(.
//
//	23-nov-91   bjw   Initial coding.
//	19-sept-93  bjw   Removed #import directives.
//


#include <stdio.h>
#include <ctype.h>
#include <time.h>

#include "classd.h"
#include "classc.h"
#include "classb.h"
#include "classa.h"
@includes()



//  Default iterations to try:
#define  ITERATIONS   40000L



////////////////////////////////////////////////////////////////////////


void
disp_parents( child )
    id    child;
{
    id    parent;

    printf( "  \"%s\" ", [child str] );

    for( parent = (id)[child superClass] ;
         parent != Nil ;
	 parent = (id)[parent superClass] )
    {
	printf( "--> %s ", [parent str] );
    }
    printf( ".\n" );
}   // disp_parents


time_t
iter_test( target, iters, jobnumber )
    register id    target;
    long    iters;
    int     jobnumber;
{
    register long     j;
    time_t     starting;
    time_t     ending;

    printf( "  Beginning %ld iterations.. ", iters );
    if( iters >= 50000L )
        printf( " (this may take a while) " );
    fflush( stdout );

    starting = time( NULL );
    for( j=iters ; --j >= 0 ; )
    {
	[target count];  	// Have 'Object' to handle method.
    }
    ending = time(NULL);

    printf( " ...done with test #%d.\n", jobnumber );
    return( (time_t) (ending - starting));
}   // iter_test


void
time_test( target, iters, job )
    id    target;
    long  iters;
    int   job;
{
    struct tm   *lasting;
    time_t     delta;

    disp_parents( target );
    delta = iter_test( target, iters, job );

    //  Now convert a relative time to something useable.  For systems
    //  with timezone support, library will try to adjust local time
    //  to GMT.  Therefore we tell it that 'delta' is already GMT!
    //
    lasting = localtime( & delta );

    printf( "  --  Test #%d took %02d:%02d:%02d.%02d.  --\n", job,
    			lasting->tm_hour, lasting->tm_min,
			lasting->tm_sec, lasting->tm_hsec );

}   // time_test


int
main( argc, argv )
    int   argc;
    char  *argv[];
{
    extern BOOL   _msgTrace;
    char  buffer[60];
    id    targ;
    long  iters = ITERATIONS;

    if( argc > 1 )
    {
	if( ! isdigit(argv[1][0]) )
	{
	    printf( "?usage: %s [iterations] default=%ld\n",
	    			argv[0], iters );
	    exit( 1 );
	}
        iters = atol( argv[1] );
    }
    printf( "Iterations = %ld.\n\n", iters );

    printf( "\nTEST #1 = Messages to class AClass.\n" );
    targ = [AClass new];
    time_test( targ, iters, 1 );

    [targ free];

    printf( "\nTEST #2 = Messages to class DClass.\n" );
    targ = [DClass new];
    time_test( targ, iters, 4 );

    [targ free];

    printf( "\nHit any key to display messaging stats: " ); fflush(stdout);
    fgets( buffer, 50, stdin );
    _msgTrace = -1;
    return( 0 );
}   // main

