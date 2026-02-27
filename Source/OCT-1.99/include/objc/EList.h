/*  ELIST.H                                      Stu F, Nov 25 1989 */

//    19-sept-93   bjw    Changed and added protected from #import.


#ifndef _OBJC_ELIST_H
# define _OBJC_ELIST_H  1

#include <objc/Object.h>

/*
 * Fields in the list object class should not be accessed directly, since
 * they are funky pointers that need interpretation.
 */
@interface EList : Object
{
	char	*head;
	char	*nothing;
	char	*tail;
}


+ new ;
- free ;
- (int) count ;
- addHead: elt ;
- addTail: elt ;
- first ;
- last ;
- remHead ;
- remTail ;

@end

#endif
