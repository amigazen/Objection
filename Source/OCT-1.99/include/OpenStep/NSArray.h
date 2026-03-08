/*
 * NSArray.h -- OpenStep immutable array interface.
 */

#ifndef _OPENSTEP_NSARRAY_H
# define _OPENSTEP_NSARRAY_H 1

# include <OpenStep/NSObject.h>

@interface NSArray : NSObject
{
  id    *_objects;
  unsigned int _count;
  unsigned int _capacity;
}

+ (id) array;
+ (id) arrayWithObject: (id)anObject;
+ (id) arrayWithObject: (id)first withObject: (id)second;
+ (id) arrayWithObjects: (id)firstObj, ...;

- (id) initWithObjects: (id)firstObj, ...;
- (void) dealloc;

- (unsigned int) count;
- (id) objectAtIndex: (unsigned int)index;

- (id) firstObject;
- (id) lastObject;
- (BOOL) containsObject: (id)anObject;

- (id) description;

@end

#endif
