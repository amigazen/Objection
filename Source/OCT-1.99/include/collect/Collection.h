//    Class Interface: Collection

#ifndef _INC_COLLECT_COLLECTION_H
# define  _INC_COLLECT_COLLECTION_H  1

#include  <objc/Object.h>
#include  <objc/IdArray.h>


@interface Collection : Object
{
	id  	contents;
}	

+ new: (int)size ;
+ with: (int)nArgs vaDcl:args ;
- freeContents ;
- free ;
+ initialize ;
- expand ;

- add: newObject ;
- addContentsTo: aCollection ;
- addContentsOf: aCollection ;
- remove: anObject ;
- removeContentsOf: aCollection ;
- eachElement ;
- eachElementPerform: (SEL) aSelector ;
- eachElementPerform: (SEL) aSelector with:arg1 ;
- eachElementPerform: (SEL) aSelector with:arg1 with:arg2 ;
- asSet ;
- asOrderedCollection ;
- (int) count ;
- (int) capacity ;
- (BOOL) isEmpty ;
- (BOOL) contains: anObject ;
- (BOOL) isCopyOf: anObject ;
- (unsigned) hash ;

- find: anObject ;
- findMatching: anObject ;
- findSTR: (STR) aString ;
- (int) offsetOf: anObject ;
- (int) offsetMatching: anObject ;
- (int) offsetSTR: (STR)aString ;

@end

/* ------------------  Definitions of Structure  ----------------------- */

typedef struct
{	@defs( IdArray )  id array[1];
}  IDARRAY ;


/*  These are eechy; very structure dependent: but use them!  */
#define  CAPACITY(id)		(((IDARRAY *)(id))->capacity)
#define  ARRAY(id)			(((IDARRAY *)(id))->array)

#endif

//	end of Collection.h

