/*  class @interface :  IdArray                                  */

#ifndef _OBJC_IDARRAY_H
# define  _OBJC_IDARRAY_H 	1

#include <objc/Object.h>

@interface IdArray : Object
{
	short  capacity;
}


/* -------------------  Public Methods  ------------------- */

+ new: (int) nElements ;
+ with: (int) nArgs vaDcl: arglist ;
+ (int) ndxVarSize ;
+ (STR) ndxVarType ;
- freeContents ;
- (int) capacity ;
- (int) count ;

- at: (int) anOffset ;
- at:(int) anOffset put:anObject ;
- add: anObject ;
- addContentsTo: aList ;
- packContents ;
- removeAt: (register int) anOffset ;
- remove: anObject ;
- removeContentsFrom: aList ;
- (unsigned) hash ;
- capacity: (int) nSlots ;
- (BOOL) isEqual: anObject ;
- (BOOL) isCopyOf: anObject ;
- boundsViolation: (int) anOffset ;

- eachElement ;
- eachElementPerform: (SEL) aSelector ;
- eachElementPerform: (SEL) aSelector with: arg1 ;
- eachElementPerform: (SEL) aSelector with: arg1 with: arg2 ;
- find: anObject ;
- findMatching: anObject ;
- findSTR: (STR) aStr ;
- (STR) describe ;
- (int) offsetOf: anObject ;
- (int) offsetMatching: anObject ;
- (int) offsetSTR: (STR) aStr ;

@end

#endif

