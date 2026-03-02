/*
 * Queue.h -- Double-ended queue; order of addition preserved.
 * Add at either end (add: at tail; addAtFront: at head). Remove by object
 * packs contents. Subclass of Collection; same structure as OrderedCollection.
 */

#ifndef _INC_COLLECT_QUEUE_H
#define _INC_COLLECT_QUEUE_H 1

#include <collect/Collection.h>

@interface Queue : Collection
{
  short firstEmptySlot;  /* next free index (logical size) */
}

- expand;
- add: newObject;
- addAtFront: newObject;
- at: (int) offset;
- at: (int) offset put: obj;
- firstElement;
- lastElement;
- addContentsOf: aCollection;
- (int) count;
- packContents;
- remove: anObject;
- boundsViolation: (int) anOffset;

@end

#endif
