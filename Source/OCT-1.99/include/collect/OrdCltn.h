//    class interface: OrderedCollection (OrdCltn)

#ifndef _INC_COLLECT_ORDCLTN_H
#define _INC_COLLECT_ORDCLTN_H  1

#include  <collect/Collection.h>

@interface  OrderedCollection : Collection
{
	short   firstEmptySlot;    // An always available offset
}


- expand ;
- add: newObject ;
- at: (int) offset ;
- at: (int) offset put: obj ;
- firstElement ;
- lastElement ;
-addContentsOf: aCollection ;
- (int) count ;
- packContents ;
- remove: anObject ;
- boundsViolation: (int)anOffset ;

@end
#endif

