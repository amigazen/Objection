//    class interface: Sequence

#ifndef _INC_COLLECT_SEQUENCE_H
#define _INC_COLLECT_SEQUENCE_H  1

#include <collect/Collection.h>

@interface  Sequence : Collection
{
	short    offset;		/* Current within sequence */
}

+ array: anIdArray ;
- next ;
- previous ;
- toFirst ;
- toLast ;
- rewind ;
- first ;
- last ;
- (int) count ;

@end

#endif

