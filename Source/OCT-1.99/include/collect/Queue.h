//    class interface: OrderedCollection (OrdCltn)

#include <collect/Collection.h>

@interface  OrderedCollection : Collection
{
	short   firstEmptySlot;    // An always available offset
}


- expand ;
- add: newObject ;
- at: (int) offset ;
- at: (int) offset put: obj ;
-addContentsOf: aCollection ;
- (int) count ;
- packContents ;
- remove: anObject ;
- boundsViolation: (int)anOffset ;

@end

