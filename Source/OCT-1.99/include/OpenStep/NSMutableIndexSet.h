/*
 * NSMutableIndexSet.h -- OpenStep mutable set of integer indexes.
 */

#ifndef _OPENSTEP_NSMUTABLEINDEXSET_H
# define _OPENSTEP_NSMUTABLEINDEXSET_H 1

# include <OpenStep/NSIndexSet.h>

@interface NSMutableIndexSet : NSIndexSet

+ (id) indexSetWithCapacity: (unsigned int)capacity;

- (id) initWithCapacity: (unsigned int)capacity;

- (void) addIndex: (unsigned int)value;
- (void) addIndexesInRange: (NSRange)range;
- (void) removeIndex: (unsigned int)value;
- (void) removeIndexesInRange: (NSRange)range;
- (void) removeAllIndexes;

@end

#endif
