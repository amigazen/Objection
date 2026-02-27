/*
 * SimPlotter.m - Objective-C ver 4.0 source module
 *              - (c) Stepstone 1990. All rights reserved.
 *              - Andrew Novobilski
 *              - December 14, 1990
 *
 * You are welcome to modify and distribute this code as long as:
 * a) you retain this header.
 * b) you make your modifications available to others.
 *
 * This class is responsible for providing the view for the output
 * of the simulation model onto a standard plotter.  This class is 
 * an example of providing an interface between an application and 
 * a standard resource.
 *
 */

#import "objc.h"
#import "SimPlotter.h"
#import "StdPlotter.h"

@implementation SimPlotter : Object
{
	id plotter;
}

/*
 * The method create an instance of SimPlotter for a particular
 * instance of a standard plotter.  At this time, ownership (and
 * the responsibility for removing the plotter when it is no
 * longer needed) belongs to the SimPlotter.
 *
 */

+createFor: aPlotter
{
	id newInstance = [self create];
	[newInstance plotter:aPlotter];
	return newInstance;
}

-plotter: aPlotter
{
	if (plotter)
		[plotter destroy];
	plotter = aPlotter;
	return self;
}

/*
 * The following methods implement the protocol for the simulation
 * view.
 *
 */

-start
{
	[plotter penDown];
}

-end
{
	[plotter penUp];
}

/*
 * This method converts the output of the GearModel (-1.0 to +1.0) 
 * to the StdPlotter (0 to 999).
 *
 * The instance variable plotter is cast to instruct the compiler
 * on which method definition to use for typing information.
 *
 */

-x:(double)anX y:(double)anY
{
	[(StdPlotter *)plotter x:(int)(((anX+1.0)/2)*999)
			       y:(int)(((anY+1.0)/2)*999) ];
	return self;
}

@end
