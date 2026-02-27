//		Class Interface: Array.m

//    19-sept-93   bjw    Changed and added protected from #import.

#ifndef _OBJC_ARRAY_H
# define _OBJC_ARRAY_H  1

#include <objc/Object.h>

@interface  Array : Object
{
	short	capacity;		/* How many storing now */
}

+ new: (int) nElements ;
+ with: (int) nArgs vaDcl: arglist ;
+ (STR) ndxVarType ;
+ (int) ndxVarSize ;
- (STR) describe ;
- (int) count ;
- (int) capacity ;
- (unsigned) hash ;
- capacity: (int) nSlots ;
- (BOOL) isEqual: anObject ;
- (BOOL) isCopyOf: anObject ;
- boundsViolation: (int) anOffset ;

@end

#endif

