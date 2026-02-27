/*
 * simMain.m - Objective-C ver 4.0 source module
 *           - (c) Stepstone 1990. All rights reserved.
 *           - Andrew Novobilski
 *           - December 14, 1990
 *
 * You are welcome to modify and distribute this code as long as:
 * a) you retain this header.
 * b) you make your modifications available to others.
 *
 * This module contains the main() routine for the interlocking
 * wheels simulation.  It acts as the "controller" (in the M/V/C
 * sense.  Not much attection has been paid to incorrect parameter
 * checking, etc. since this is a lab toy, and not a production
 * application.  Sue me :-).
 *
 */

#import "objc.h"
#import "GearModel.h"
#import "SimPlotter.h"
#import "StdPlotter.h"

main( argc, argv )
int argc;
char *argv[];
{
	id model, view; //this routine serves as controller
	int cir, off;

/*
 * The next several lines of code insure that parameter count and
 * values are correct.
 *
 */

	if ( argc !=3 ) 
	{
		printf( "usage: simDemo <circumference> <offset>\n" );
		exit( 0 );
	}

	cir = atoi(argv[1]); 
	off = atoi(argv[2]); 
	if ( (cir<1) || (cir>360) )
	{
		printf( "error: 1 <= circumference <= 360\n" );
		exit( 0 );
	}
	else if ( (off<0) || (off>100) )
	{
		printf( "error: 0 <= offset <= 100\n" );
		exit( 0 );
	}

/*
 * build the simulation.
 *
 */

	view  = [SimPlotter createFor:[StdPlotter create]];
	model = [GearModel  create];
	[model xyAttachTo:view];

/*
 * run the simulation.
 *
 */

	[model circumference:cir offset:off];
	[view start];
	[model cycle];
	[view end];

/*
 * cleanup
 *
 */

	[model destroy];
	[view  destroy];

	exit( 0 );
}

