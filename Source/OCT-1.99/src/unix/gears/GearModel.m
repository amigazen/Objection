/*
 * GearModel.m - Objective-C ver 4.0 source module
 *            - (c) Stepstone 1990. All rights reserved.
 *            - Andrew Novobilski
 *            - December 14, 1990
 *
 * You are welcome to modify and distribute this code as long as:
 * a) you retain this header.
 * b) you make your modifications available to others.
 *
 * This class implements the actual model of the interlocked wheels
 * simulation.  It relies on the components used to construct the
 * class to obey the agreed upon message conventions.
 */

#import "objc.h"
#import "GearModel.h"
#import "Clock.h"
#import "Wheel.h"

@implementation GearModel : Object
{
	id fixedWheel;
	id mobileWheel;
	id clock;
	id xyReceiver;
	double currentX, currentY;
}

/*
 * The initialize method will create the components necessary to
 * construct the GearModel.  After the components are created, they
 * are attached in the correct order.  Finally, the reset message
 * is sent to place the GearModel into a known state.
 *
 */

-initialize 
{
      [super initialize];
      fixedWheel = [Wheel create];
      mobileWheel = [Wheel create];
      clock = [Clock create];
      [clock turnAttachTo: fixedWheel];
      [fixedWheel OxOyAttachTo: mobileWheel];
      [fixedWheel turnAttachTo: mobileWheel];
      [mobileWheel OxOyAttachTo: self];
      [mobileWheel turnAttachTo: self];
      [self reset];
      return self;
}

-reset 
{
	currentX = currentY = 0.;
	[super reset];
	[fixedWheel reset];
	[mobileWheel reset];
	[clock reset];
	return self;
}

-free 
{
      [clock destroy];
      [fixedWheel destroy];
      [mobileWheel destroy];
      return [super free];
}

/*
 * The following method is used by consumers of GearModel to attach
 * the output of GearModel to the correct receiver.
 *
 */

-xyAttachTo: aReceiver 
{
	xyReceiver = aReceiver;
	return self;
}

/*
 * The circumference refered to in the following method is that of
 * the mobile wheel.  For simplicity, the circumference for circle
 * of unit radius is 360 units.  In addition, the fixed wheel is 
 * always given a circumference of 360.  The offset refers to the 
 * distance from the origin of the fixed wheel to the origin of the 
 * mobile wheel.
 *
 * This method then computes the correct radii for both wheels that
 * will simulate the correctly model the interlocking wheels.  The 
 * period is the number of increments the mobile wheel must be turned 
 * so that its tracking point returns to the same location that it 
 * started at.
 *
 */

static double maxCircum = 360.;
static double maxOffset = 100.;

static int computePeriod( circum1, circum2 )
int circum1, circum2;
{
	int per;

	if (circum1 > circum2 )
	{
		per = circum1;
		circum1 = circum2;
		circum2 = per;
	}
	for (per = circum1; per % circum2; per += circum1);
	return per;
}

/*
 * The x:y: method definition is here for the compilers use in
 * performing type checking.
 *
 */

-x:(double)anX y:(double)anY {}

-circumference: (int)cir offset:(int)off
{
	double mobileRadius = cir / maxCircum;

	[fixedWheel circumference:360 offset:(int)(100*(1.-mobileRadius))];
	[mobileWheel circumference:cir offset:off];
	[fixedWheel Ox:0. Oy:0.];
	[clock period:computePeriod( 360, cir )];
	[xyReceiver x:currentX y:currentY ];
	return self;
}


/*
 * The following message is immediatly passed on to the clock.
 *
 */

-cycle 
{
	[clock cycle];
	return self;
}

/*
 * The next two methods are used to generate output from the
 * GearModel to the output receiver, whenever the interlocking
 * wheels are turned.  These methods are expected to be
 * present by the mobileWheel, which has had its output directed
 * back to an internal method in the GearModel.
 *
 */

-Ox:(double)x Oy:(double)y
{
	currentX = x;
	currentY = y;
	return self;
}

-turn:(int)numberOfUnits
{
	[xyReceiver x:currentX y:currentY];
	return self;
}

@end
