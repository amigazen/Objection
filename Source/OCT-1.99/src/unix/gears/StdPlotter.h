/*
 * StdPlotter.h - Objective-C ver 4.0 source module
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
#import "BBObject.h"

@interface StdPlotter : Object
{
	BOOL penIsDown;
	long Ex, Ey;
	double cfX, cfY;
}

-initialize;
-reset;
-free;

-x:(int)newX y:(int)newY;
-penUp;
-penDown;

@end
