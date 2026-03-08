/*
 * NSMutableSet.h -- OpenStep mutable set interface.
 */

#ifndef _OPENSTEP_NSMUTABLESET_H
# define _OPENSTEP_NSMUTABLESET_H 1

# include <OpenStep/NSSet.h>

@interface NSMutableSet : NSSet

+ (id) setWithCapacity: (unsigned int)capacity;

- (id) initWithCapacity: (unsigned int)capacity;

- (void) addObject: (id)anObject;
- (void) removeObject: (id)anObject;
- (void) removeAllObjects;

@end

#endif
