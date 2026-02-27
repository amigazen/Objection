/*
 * SimPlotter.h - Objective-C ver 4.0 source module
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
#import "BBObject.h"

@interface SimPlotter : Object
{
	id plotter;
}

+createFor: aPlotter;
-plotter: aPlotter;
-start;
-end;
-x:(double)anX y:(double)anY;

@end
