/*
 * GearModel.h - Objective-C ver 4.0 source module
 *            - (c) Stepstone 1990. All rights reserved.
 *            - Andrew Novobilski
 *            - December 14, 1990
 *
 * You are welcome to modify and distribute this code as long as:
 * a) you retain this header.
 * b) you make your modifications available to others.
 *
 * This is the class interface for the actual model of the interlocked 
 * wheels simulation.  It relies on the components used to construct 
 * the class to obey the agreed upon message conventions.
 *
 */

#import "objc.h"
#import "BBObject.h"

@interface GearModel : Object
{
	id fixedWheel;
	id mobileWheel;
	id clock;
	id xyReceiver;
	double currentX, currentY;
}

-initialize;
-reset;
-free;
-xyAttachTo: aReceiver;
-circumference:(int)cir offset:(int)off;
-cycle;
-Ox:(double)x Oy:(double)y;
-turn:(int)numberOfUnits;

@end
