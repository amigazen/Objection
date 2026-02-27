/*
 * Clock.h - Objective-C ver 4.0 source module
 *         - (c) Stepstone 1990. All rights reserved.
 *         - Andrew Novobilski
 *         - December 14, 1990
 *
 * You are welcome to modify and distribute this code as long as:
 * a) you retain this header.
 * b) you make your modifications available to others.
 *
 * This class implements a clock device that accepts a period and
 * turn size as input and produces the appropriate number of turn 
 * messages, with the correct turn amount on its output when it
 * receives the cycle message.
 *
 */

#import "objc.h"
#import "BBObject.h"

@interface Clock : Object
{
	id turnReceiver;
	int period, turnAmount;
}

-initialize;
-turnAttachTo: aReceiver;
-period: (int) per;
-turnAmount: (int) amt;
-cycle;

@end
