//    class interaface: Set

#ifndef _INC_COLLECT_SET_H
#define _INC_COLLECT_SET_H  1

#include  <collect/Collection.h>

@interface  Set : Collection
{
	short   	tally;			/* Unique items we hold */
}


- expand ;
- add: newObject ;
- filter: newObject ;
- addContentsTo: aCollection ;
- union: aCollection ;
- intersection: aCollection ;
- difference: aCollection ;
- (int) count ;
- find: anObject ;
- (BOOL) contains: anObject ;
- remove: anObject ;

@end
#endif

