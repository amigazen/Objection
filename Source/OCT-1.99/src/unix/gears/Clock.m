/*
 * Clock.m - Objective-C ver 4.0 source module
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
#import "Clock.h"

@implementation Clock : Object
{
	id turnReceiver;
	int period, turnAmount;
}

-initialize
{
	[super initialize];
	period = 0;
	turnAmount = 1;
	return self;
}

/*
 * Methods for attaching output to the appropriate receiver.
 *
 */

-turnAttachTo: aReceiver
{
	turnReceiver = aReceiver;
	return self;
}

/*
 * The following methods are used to set the period and 
 * turn amount that determines how the clock sends messages
 * to its output during the cycle message.
 *
 */

-period: (int) per
{
	period = per;
	return self;
}

-turnAmount: (int) amt
{
	turnAmount = amt;
	return self;
}

/*
 * The cycle method uses the period and turnAmount instance
 * variables to compute the exact number of turn: messages
 * (including the amount to turn) that need to be sent to
 * the output receiver.
 *
 * The 'turn:' dummy method is added to provide a forward
 * reference for the compiler to use for the type checking
 * of the trun: message sent in the cycle method
 *
 */

-turn:(int)amt {}

-cycle
{
	int cnt;

	for (cnt = period; cnt > turnAmount; cnt -= turnAmount )
		[turnReceiver turn:turnAmount];
	
	if ( cnt )
		[turnReceiver turn:turnAmount];
	
	return self;
}

@end
