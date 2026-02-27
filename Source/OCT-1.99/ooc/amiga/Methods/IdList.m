/*   class:  IdList   Ordered Sequence of objects           April 90, bjw */

/*
//		This class mimics Stuf Furgeson's IdList class that's probably
//		based on his Exec List/Node classes.  Since he didn't send me
//		source to this class, I rolled my own.  The interface here must
//		match that of IdList.
//
//		13-april-90	bjw 	Initial copying.
*/

#include <apps/IdList.h>

@implementation IdList : List ;


/* ------------------------  Accessing Methods  ----------------------- */


//  Make this identical..  Returns first element:
- toFirst
{
	return [self first];
}


@end

