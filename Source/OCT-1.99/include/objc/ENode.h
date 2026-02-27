/*   ENODE.H                                         Stu F.  Nov 25,1989 */

//    19-sept-93   bjw    Changed and added protected from #import.

#ifndef _OBJC_ENODE_H
# define _OBJC_ENODE_H  1

#include <objc/Object.h>

/*
 * Fields in the Node class should not be accessed directly since
 * they need to be de-funkified to get at the real object referenced.
 */
@interface ENode : Object
{
	char	*next;
	char	*prev;
}


- next ;
- previous ;
- insertAfter: elt ;
- remove ;

@end

#endif

