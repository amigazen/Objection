/*
 * BBObject.h - Objective-C ver 4.0 source module
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
#import "Object.h"

@interface BBObject : Object
{
}

+initialize;
+create;
-initialize; 
-reset;
-destroy;
-free;

@end
