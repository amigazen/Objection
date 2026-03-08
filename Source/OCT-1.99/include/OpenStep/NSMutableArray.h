/*
 * NSMutableArray.h -- OpenStep mutable array interface.
 */

#ifndef _OPENSTEP_NSMUTABLEARRAY_H
# define _OPENSTEP_NSMUTABLEARRAY_H 1

# include <OpenStep/NSArray.h>

@interface NSMutableArray : NSArray

+ (id) arrayWithCapacity: (unsigned int)capacity;

- (id) initWithCapacity: (unsigned int)capacity;

- (void) addObject: (id)anObject;
- (void) insertObject: (id)anObject atIndex: (unsigned int)index;
- (void) removeObjectAtIndex: (unsigned int)index;
- (void) removeLastObject;
- (void) removeAllObjects;
- (void) replaceObjectAtIndex: (unsigned int)index withObject: (id)anObject;

@end

#endif
