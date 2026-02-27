/*  class @interface :  LinkList                                     */

/*
 *    21-mar-91   bjw    changed from class List to class LinkList 
 *    19-sept-93   bjw    Changed and added protected from #import.
 */

#ifndef _OBJC_LINKLIST_H
# define _OBJC_LINKLIST_H  1


#include <objc/Object.h>


@interface LinkList : Object
{
	id  	first, current, last;
}

/* -------------------  Public Methods  ------------------- */

+ new;
- free ;
- freeContents ;
- freeObjects ;

- add: anitem ;
- addFirst: anitem ;
- addLast: anitem ;
- addTail: anitem ;
- remove:anObject ;
- removeFirst ;
- removeLast ;
- remove ;

- toFirst ;
- first ;
- next ;
- toLast ;
- eachElementPerform:(SEL)sel with:arg ;

- find:anObject ;
- findMatching:anObject ;
- findSTR:(STR)aStr ;

- (BOOL) isEmpty ;
- (BOOL) contains:anObject ;
- (int) count ;

@end

#endif

