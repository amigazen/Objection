/*
 * BBObject.m - Objective-C ver 4.0 source module
 *           - (c) Stepstone 1990. All rights reserved.
 *           - Andrew Novobilski
 *           - December 14, 1990
 *
 * You are welcome to modify and distribute this code as long as:
 * a) you retain this header.
 * b) you make your modifications available to others.
 *
 * BBObject illustrates the use of poseAs: to insert the behaviors 
 * that the black box simulation required all classes to inherit.
 *
 * Notice that there are no instance variables listed.  This is a
 * requirement imposed by the use of poseAs.
 */

#import "objc.h"
#import "BBObject.h"
#import "Object.h"

@implementation BBObject : Object
{
}

/*
 * The actual posing is done by sending the "poseAs" message in
 * the "+initialize" factory method.  This allows runtime insertion
 * of behaviors to an existing library.
 */

+initialize
{
	static BOOL beenHere = NO;
	if ( !beenHere ) {
		beenHere = YES;
		[self poseAs:Object];
		}
	return self;
}

/*
 * The "create" method is the standard way for creating an instance
 * of an object required in the simulation.  It relies upon the
 * "new" method inherited from Object to allocate the space.
 *
 * Once the object is created, it is sent an initialize message. This
 * protocol allows a new class to be built by using the inherited
 * creation methods, and overriding the "initialize" method for the
 * custome work the subclass requires.
 */

+create 
{
	id newInstance = [self new];
	[newInstance initialize];
	return newInstance;
}

-initialize 
{
      return self;
}

/*
 * The "-reset" method is used to bring the black box back to a
 * known state.
 */

-reset 
{
	return self;
}

/*
 * The "-destroy" method is used to request an instace to destroy
 * itself.  It is implemented by using the inherited free method
 * to actually remove the object from the system.
 *
 * Subclasses of objects override the free method to remove
 * objects they have created during initialization.
 */

-destroy
{
	return [self free];
}

-free 
{
      return [super free];
}

@end
