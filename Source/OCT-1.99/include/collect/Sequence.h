//    class interface: Sequence

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
- first ;
- last ;
- (int) count ;

@end

