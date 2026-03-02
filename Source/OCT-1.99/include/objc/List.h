/*  class @interface :  List                                     */
/*
 *    19-sep-93     bjw     Use #include instead of #import.
 */

#ifndef  _OBJC_LIST_H
# define  _OBJC_LIST_H  1

#include  <objc/Object.h>

@interface List :  Object
{
    id    *dataPtr;
    unsigned   numElements, maxElements;
}


/* -------------------  Public  Methods  ------------------- */

+ new ;
+ newCount:(unsigned)numSlots ;
- free ;
- freeObjects ;
- copy ;
- (BOOL) isEqual: otherList ;
- (int) capacity ;
- setAvailableCapacity: (unsigned)numSlots ;
- (int) count ;
- objectAt: (unsigned)index ;
- (unsigned) indexOf: matchObject ;
- lastObject ;
- first ;
- addObject: newElement ;
- addObjectIfAbsent: testObject ;
- insertObject: newElement at: (unsigned) index ;
- removeObject: targetObj ;
- removeObjectAt: (unsigned) index ;
- removeLastObject ;
- empty ;
- replaceObject: anElement with: newElement ;
- replaceObjectAt: (unsigned)index with: newElement ;
- makeObjectsPerform: (SEL)aSel ;
- makeObjectsPerform: (SEL)aSel with:anObj ;


/* -------------------  Private Methods  ------------------- */


#define  NX_NOT_IN_LIST   (~0)

@end

#endif

