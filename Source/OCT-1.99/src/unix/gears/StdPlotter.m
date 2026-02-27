/*
 * StdPlotter.m - Objective-C ver 4.0 source module
 *              - (c) Stepstone 1990. All rights reserved.
 *              - Andrew Novobilski
 *              - December 14, 1990
 *
 * You are welcome to modify and distribute this code as long as:
 * a) you retain this header.
 * b) you make your modifications available to others.
 *
 * This class encapsulates the standard plotting functionality contained
 * in Sun's libplot.a.  It currently implements the methods necessary
 * to respond to the messages the interlocked wheel simulation expects.
 *
 */

#import "objc.h"
#import "StdPlotter.h"

#define MAX_LOGICAL_SIZE 1000
#define MAX_REAL_SIZE     500
#define SPACE_OFFSET        5

@implementation StdPlotter : Object
{
	BOOL penIsDown;
	long Ex, Ey;
	double cfX, cfY;
}

/*
 * The initialize method opens the plot devices and creates a virtual
 * tablet with an extent of 1000,1000 units.  The pen is up and it is
 * at location 0, 0.
 *
 * The free method will be used to close the plotting device when the 
 * application finishes.
 */

-initialize 
{
	[super initialize];
	openpl();
	Ex = Ey = MAX_REAL_SIZE;
	cfX = ((double)Ex)/MAX_LOGICAL_SIZE;
	cfY = ((double)Ey)/MAX_LOGICAL_SIZE;
	space(0,0,Ex+2*SPACE_OFFSET,Ey+2*SPACE_OFFSET);
	penIsDown = NO;
	return self;
}

-reset 
{
	[super reset];
	erase();
	penIsDown = NO;
	return self;
}

-free 
{
	closepl();
	return [super free];
}


/*
 * The following methods implement the functionality needed to
 * make instances of this class behave like a plotter.
 */

-x:(int)newX y:(int)newY
{
	if (penIsDown)
		cont( (((int)(newX*cfX))+SPACE_OFFSET),
	              (((int)(newY*cfY))+SPACE_OFFSET) );
	else
		move( (((int)(newX*cfX))+SPACE_OFFSET),
	              (((int)(newY*cfY))+SPACE_OFFSET) );
	return self;
}

-penUp
{
	penIsDown = NO;
}

-penDown
{
	penIsDown = YES;
}

@end
