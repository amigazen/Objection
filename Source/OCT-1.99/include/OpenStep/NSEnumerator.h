/*
 * NSEnumerator.h -- OpenStep enumerator interface (iterate over collection).
 */

#ifndef _OPENSTEP_NSENUMERATOR_H
# define _OPENSTEP_NSENUMERATOR_H 1

# include <OpenStep/NSObject.h>

@interface NSEnumerator : NSObject
{
  id             _collection;
  unsigned int   _index;
}

- (id) initWithArray: (id)array;
- (id) nextObject;
- (void) dealloc;

@end

#endif
