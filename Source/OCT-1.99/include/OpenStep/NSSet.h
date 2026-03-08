/*
 * NSSet.h -- OpenStep immutable set (unique objects) interface.
 */

#ifndef _OPENSTEP_NSSET_H
# define _OPENSTEP_NSSET_H 1

# include <OpenStep/NSObject.h>

@interface NSSet : NSObject
{
  id    *_objects;
  unsigned int _count;
  unsigned int _capacity;
}

+ (id) set;
+ (id) setWithObject: (id)anObject;
+ (id) setWithObjects: (id)firstObj, ...;

- (id) initWithObjects: (id)firstObj, ...;
- (void) dealloc;

- (unsigned int) count;
- (id) member: (id)anObject;
- (BOOL) containsObject: (id)anObject;

- (id) description;

@end

#endif /* _OPENSTEP_NSSET_H */
