/*
 * Wheel.m - Objective-C ver 4.0 source module
 *         - (c) Stepstone 1990. All rights reserved.
 *         - Andrew Novobilski
 *         - December 14, 1990
 *
 * You are welcome to modify and distribute this code as long as:
 * a) you retain this header.
 * b) you make your modifications available to others.
 *
 * This class implements the the Wheel model.  Each wheel has a
 * radius and an origin.  The wheel initially has its track point
 * set to (radius, 0.).  Each time the wheel receives the turn
 * message, it recomputes a new value for the track point, notifies
 * a predeterined receiver that the point has changed, and then
 * passes the turn: message on.
 *
 * The wheel can turn from 0 to 360 units. The angle will be kept
 * in degrees, while a second instance variable will keep the angle
 * in radians.
 *
 */

#import "objc.h"
#import "Wheel.h"
#import <math.h>

@implementation Wheel : Object
{
	id OxOyReceiver, turnReceiver;
	double radius;
	double Ox, Oy;
	double angle, theta, degreesPerTurn;
}


/*
 * The following methods attach the output from Wheel to an
 * appropriate receiver.
 *
 */

-OxOyAttachTo: aReceiver
{
	OxOyReceiver = aReceiver;
	return self;
}

-turnAttachTo: aReceiver
{
	turnReceiver = aReceiver;
	return self;
}


/*
 * The following methods manage the wheel model
 *
 */

static double maxCir = 360.;
static double maxOff = 100.;

-circumference:(int)cir offset:(int)off
{
	radius = (cir / maxCir)*(off/maxOff);
	degreesPerTurn = maxCir / cir;
	angle = theta = 0.;
	return self;
}

-Ox:(double)x Oy:(double)y
{
	Ox = x;
	Oy = y;
	[OxOyReceiver Ox:(Ox+radius*cos(theta))
		      Oy:(Oy+radius*sin(theta)) ];
	return self;
}

static double convert = 0.017453; // 2Pi/360

-turn:(int)turnAmount
{
	angle += turnAmount*degreesPerTurn;
	if ( angle <= -180. )
		angle += 360.;
	else if ( angle > 180. )
		angle -= 360.;

	theta = angle * convert;
	[OxOyReceiver Ox:(Ox+radius*cos(theta))
		      Oy:(Oy+radius*sin(theta)) ];
	[turnReceiver turn:-turnAmount];
	return self;
}

@end
