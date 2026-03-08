/*
 * NSIndexSet.h -- OpenStep immutable set of integer indexes.
 */

#ifndef _OPENSTEP_NSINDEXSET_H
# define _OPENSTEP_NSINDEXSET_H 1

# include <OpenStep/NSObject.h>
# include <OpenStep/NSRange.h>

@interface NSIndexSet : NSObject
{
  unsigned int *_indexes;
  unsigned int  _count;
  unsigned int  _capacity;
}

+ (id) indexSet;
+ (id) indexSetWithIndex: (unsigned int)value;
+ (id) indexSetWithIndexesInRange: (NSRange)range;

- (id) initWithIndex: (unsigned int)value;
- (id) initWithIndexesInRange: (NSRange)range;
- (id) initWithIndexSet: (id)indexSet;
- (void) dealloc;

- (unsigned int) count;
- (unsigned int) firstIndex;
- (unsigned int) indexGreaterThanIndex: (unsigned int)value;
- (BOOL) containsIndex: (unsigned int)value;

@end

#endif
