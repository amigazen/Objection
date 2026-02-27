/*
 * Wheel.h - Objective-C ver 4.0 source module
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
#import "BBObject.h"

@interface Wheel : Object
{
	id OxOyReceiver, turnReceiver;
	double radius;
	double Ox, Oy;
	double angle, theta, degreesPerTurn;
}


-OxOyAttachTo: aReceiver;
-turnAttachTo: aReceiver;
-circumference:(int)cir offset:(int)off;
-Ox:(double)x Oy:(double)y;
-turn:(int)turnAmount;

@end
